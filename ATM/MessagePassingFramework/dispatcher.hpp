#ifndef guard_DISPATCHER_HPP_
#define guard_DISPATCHER_HPP_

#include "message_queue.hpp"


namespace messaging
{


template<typename PreviousDispatcher, typename Msg, typename Func>
class TemplateDispatcher
{
    queue* q;
    PreviousDispatcher* prev;
    Func f;
    bool chained;

    TemplateDispatcher(const TemplateDispatcher& ) = delete;
    TemplateDispatcher& operator=(const TemplateDispatcher& ) = delete;

    // TemplateDispatcher instantiations are friends of each other
    template<typename Dispatcher, typename OtherMsg, typename OtherFunc>
    friend class TemplateDispatcher;

    void wait_and_dispatch()
    {
        for(;;)
        {
            auto msg = q->wait_and_pop();
            if(dispatch(msg))
                break;
        }
    }

    bool dispatch(const std::shared_ptr<message_base>& msg)
    {
        if(wrapped_message<Msg>* wrapper =
            dynamic_cast<wrapped_message<Msg>*>(msg.get())){
            f(wrapper->contents);
            return true;
        }
        else{
            return prev->dispatch(msg);
        }
    }

public:
    TemplateDispatcher(TemplateDispatcher&& other)
        : q(other.q), prev(other.prev), f(std::move(other.f)),
          chained(other.chained)
        {
            other.chained = true;
        }
    TemplateDispatcher(queue* q_, PreviousDispatcher* prev_, Func&& f_)
        : q(q_), prev(prev_), f(std::forward<Func>(f_)), chained(false)
        {
            prev_->chained = true;
        }

    template<typename OtherMsg, typename OtherFunc>
    TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>
    handle(OtherFunc&& of)
    {
        return TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>(
            q, this, std::forward<OtherFunc>(of));
    }

    ~TemplateDispatcher() noexcept(false)
    {
        if(!chained){
            wait_and_dispatch();
        }
    }
};


class close_queue
{ };

class dispatcher
{
    queue* q;
    bool chained;

    dispatcher( const dispatcher& ) = delete;
    dispatcher& operator=(const dispatcher&) = delete;

    template<
        typename Dispatcher,
        typename Msg,
        typename Func>
    friend class TemplateDispatcher;

    void wait_and_dispatch()
    {
        for(;;){
            auto msg=q->wait_and_pop();
            dispatch(msg);
        }
    }

    bool dispatch( const std::shared_ptr<message_base>& msg )
    {
        if(dynamic_cast<wrapped_message<close_queue>*>(msg.get()))
        {
            throw close_queue();
        }
    }

public:
    dispatcher( dispatcher&& other )
        : q(other.q), chained(other.chained)
    {
        other.chained = true;
    }

    explicit dispatcher(queue* q_)
        : q(q_), chained(false)
        { }
    
    template<typename Message, typename Func>
    TemplateDispatcher<dispatcher, Message, Func>
    handle( Func&& f )
    {
        return TemplateDispatcher<dispatcher, Message, Func>(
            q, this, std::forward<Func>(f));
    }

    ~dispatcher() noexcept(false)
    {
        if(!chained){
            wait_and_dispatch();
        }
    }
};

} // namespace messaging


#endif /* guard_DISPATCHER_HPP_ */
