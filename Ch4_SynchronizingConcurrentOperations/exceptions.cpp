#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <stdexcept>

void async_exceptions()
{
// --- if std::async() is called with the std::launch::async policy
// but the system is unable to launch the task asynchronously in its own thread
// async will throw an std::system_error with error condition
// std::errc::resource_unavailable_try_again
    
    // would throw an exception if starting a thread was impossible...
    
std::cout << "---- async exceptions ----\n" << std::endl;

    try{auto f( std::async([]{
        std::cout << "async_exceptions: " << " Hi from thread[" << std::this_thread::get_id()
            << "]" << std::endl;
        return 42; }) );
        std::cout << "async_exceptions: " << " async result = " << f.get() << std::endl;
    }
    catch(std::exception& e){ std::cerr <<e.what() << std::endl;}

// --- if an exception is thrown within the thread it will be stored in the future
// in place of the expected value, the future then becomes ready
// a call to get() will rethrow the exception
    try{
        std::future<int> f( std::async([]{
            std::cout << "async_exceptions: " << " Hi from thread[" << std::this_thread::get_id()
                << "] about to throw an exception... " << std::endl;
            throw std::runtime_error("exception from the future");
            return 42;
        }) );
        std::cout << "async_exceptions: " << " async result = " << f.get() << std::endl;
    }
    catch( std::exception& e ){ std::cerr << e.what() << std::endl; }
std::cout << "---- async exceptions done ----\n\n" << std::endl;
}

std::future<int> create_task()
{
// disaster -- create a task, return its associated future, but never call it
// this will store an exception in the future (read ahead)
    std::packaged_task<int(int)> pt(([](int i){
            std::cout << "We won't call this guy so I won't bother..." << std::endl;
            return i*42;
        }));
    auto f = pt.get_future();
    return f;
}

void packaged_task_exceptions()
{
std::cout << "**** packaged_task exceptions ****\n" << std::endl;
    // storing the thrown exception
    try{
        std::packaged_task<int(int)> pt([](int i){
            std::cout << "Hi from packaged_task[" << std::this_thread::get_id()
                << "] about to throw an exception... " << std::endl;
            throw std::runtime_error("packaged exception");
            return i;
        });
        std::future<int> f = pt.get_future();
        std::thread t(std::move(pt), 11);
        f.wait();
        t.join();
        std::cout << __FUNCTION__ << " packaged_task result = " << f.get()
            << std::endl;
    }
    catch( std::future_error& e ){ std::cerr << e.what() << std::endl;}
    catch(...){ std::cerr << "packaged_task - some other exception..." << std::endl;}
    // if a packaged_task is constructed and later destroyed without being
    // called it will store and exception of type std::future_error
    // with error code of std::future_errc::broken_promise
    try{
        std::future<int> f = create_task();
        std::cout << __FUNCTION__ << " packaged_task result = " << f.get()
            << std::endl;
    }
    catch( std::future_error& e ){ std::cerr << e.what() << std::endl; }
    catch(...){ std::cerr << "packaged_task - some other exception..." << std::endl;}

std::cout << "**** packaged_task exceptions done ****\n\n" << std::endl;
}


std::future<int> create_promise()
{
// disaster again - create a promise, return its associated future, but never
// set_value() before destroying the object. This will store a std::future_error
// exception in the future object
    std::promise<int> p;
    auto f = p.get_future();
    return f;
}

void promise_exceptions()
{
std::cout << "++++ promise exceptions +++++\n" << std::endl;
// and promisses...
    try{
        std::promise<int> pr;
        std::future<int> f = pr.get_future();
        std::thread([&pr]{
            std::cout << "Hi from promissing thread[" << std::this_thread::get_id()
                << "] about to throw an exception... " << std::endl;
            throw std::runtime_error("promissing exception");
            return 42;
        });
        f.wait();
        std::cout << __FUNCTION__ << " promise result = " << f.get() << std::endl;
    }
    catch( std::future_error& e ){ std::cerr << e.what() << std::endl; }
    catch(...){ std::cerr << "promise - some other exception..." << std::endl;}
// if an std::promise object is constructed but its value is never set
// an exception of type std::future_error with error code of
// std::future_errc::broken_promise will be stored in the future
    try{
        std::future<int> f = create_promise();
        f.wait();
        std::cout << __FUNCTION__ << " promise result = " << f.get() << std::endl;
    }
    catch( std::future_error& e ){ std::cerr << e.what() << std::endl; }
    catch(...){ std::cerr << "promise - some other exception..." << std::endl;}
std::cout << "++++ promise exceptions done +++++\n\n" << std::endl;
}


int main()
try{
    async_exceptions();
    packaged_task_exceptions();
    promise_exceptions();
}
catch(...){
    std::cerr << "we don't want to be here" << std::endl;
}