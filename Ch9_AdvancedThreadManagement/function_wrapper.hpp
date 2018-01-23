#ifndef FUNCTION_WRAPPER_HPP_
#define FUNCTION_WRAPPER_HPP_

#include <thread>
#include <memory>
#include <utility>


class function_wrapper
{
    
    struct impl_base
    {
        virtual void call() = 0;
        virtual ~impl_base() = default;
    };

// --- member variables
    std::unique_ptr<impl_base> impl;
// --- 
    template<typename Function>
    struct impl_type : public impl_base
    {
        Function func;
        impl_type(Function&& f) : func(std::move(f)) { }
        void call() { f(); }
    };

public:
    template<typename Function>
    function_wrapper(Function&& f)
        : impl(new impl_type<Function>(std::move(f)))
        { }

    void operator()() { impl->call(); }

    function_wrapper() = default;
    function_wrapper( function_wrapper&& other ) noexcept
        : impl(std::move(other.impl))
        { }
    function_wrapper& operator=(function_wrapper&& other) noexcept
    {
        impl = std::move(other.impl);
        return *this;
    }

    function_wrapper(const function_wrapper&) = delete;
    function_wrapper& operator=(const function_wrapper&) = delete;

};


#endif /* FUNCTION_WRAPPER_HPP_ */
