/* 
** In some cases - when data is rarely updated, but accessed often it is
** beneficial to provide a locking mechanism which allows concurrent
** read-only access, but dissallows concurrent acces during write operations.
** This kind of a mechanism is implemented by shared_mutex and shared_lock.
** If a shared_mutex is acquired via shared_lock, or via calling lock_shared(),
** concurrent access is allowed,
** however if a shared_mutex is locked via a lock_guard, or calling lock()
** all concurrent access is dissallowed until the mutex is released
** (lock_guard goes out of scope or unlock() is called).
*/
#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <random>
#include <map>
#include <string>
#include <utility>


struct entry
{
    std::string data;

    entry() = default;
    explicit entry( const std::string& s )
        : data(s) { }
    explicit entry( std::string&& s )
        : data(std::move(s)) { }
    bool operator==(const entry& other) const noexcept
    { return data == other.data; }
    bool operator!=(const entry& other) const noexcept
    { return !(*this == other); }
    bool operator < (const entry& other) const noexcept
    { return data < other.data; }
    const std::string& to_string() const noexcept
    { return data; }
};

class cache
{
    std::map<std::string,entry> entries;
    mutable std::shared_mutex entry_mutex;
public:
    entry find_entry( const std::string& key ) const {
        // use shared_lock to allow concurrent access, as long as strictly
        // read-only operations are ongoing (i.e. no lock_guard is used or lock called)
        std::shared_lock<std::shared_mutex> lk(entry_mutex);
        const auto it = entries.find(key);
        return (it == entries.end()) ? entry() : it->second;
    }

    void update_or_add_entry( const std::string& key, const entry& value ) {
        // use lock_guard to dissallow concurrent access while mutating the map
        std::lock_guard<std::shared_mutex> lk(entry_mutex);
        entries[key] = value;
    }
};

template<typename Action>
class periodic_action
{
    static std::default_random_engine re;
    std::uniform_int_distribution<> ud;
    Action action;
    int ticks;
public:
    explicit periodic_action(Action action_, int low, int high, int ticks_)
        : ud(low, high), action(action_), ticks(ticks_) { }
    template<typename... Args>
    void operator()(Args&&... args){
        if(ticks){
        for(int i=0; i<ticks; ++i){
            int t = ud(re);
            action(std::forward<Args>(args)...);
            std::this_thread::sleep_for(std::chrono::milliseconds(t));
        }
        }
        else{
            while(true){
                int t = ud(re);
                action(std::forward<Args>(args)...);
                std::this_thread::sleep_for(std::chrono::milliseconds(t));
            }
        }
    }
};
template<typename Action>
std::default_random_engine periodic_action<Action>::re{std::chrono::system_clock::now().time_since_epoch().count()};


int main()
{
    std::default_random_engine re{std::chrono::system_clock::now().time_since_epoch().count()};
    std::uniform_int_distribution<> ud(0,4);
    std::string keys[]{ "one", "two", "three", "four", "five" };
    std::string newkeys[]{"ONE", "TWO", "THREE", "FOUR", "FIVE"};
    cache ce;
    ce.update_or_add_entry("one", entry("1"));
    ce.update_or_add_entry("two", entry("2"));
    ce.update_or_add_entry("three", entry("3"));
    ce.update_or_add_entry("four", entry("4"));
    ce.update_or_add_entry("five", entry("5"));
    auto read_action = [&ce, &ud, &re, &keys]{
        int r = ud(re);
        std::cout << "read thread[" << std::this_thread::get_id()
            << "] ce[" << keys[r] << "] = " << ce.find_entry(keys[r]).to_string()
            << std::endl;
    };
    auto write_action = [&ce, &ud, &re, &keys,  &newkeys]{
        int r = ud(re);
        std::cout << "write thread[" << std::this_thread::get_id()
            << "] update ce[" << keys[r] << "] to " << newkeys[r] << std::endl;
        ce.update_or_add_entry(keys[r], entry(newkeys[r]));
    };

    // two threads are reading the cache frequently
    std::thread rthread1(periodic_action<decltype(read_action)>(
        read_action, 300, 500, 0));
    std::thread rthread2(periodic_action<decltype(read_action)>(
        read_action, 400, 500, 0));
    rthread1.detach(); rthread2.detach();

    // one thread is updating the data less frequently
    std::thread wthread(periodic_action<decltype(write_action)>(
        write_action, 1000, 2000, 10));
    wthread.join();
}
