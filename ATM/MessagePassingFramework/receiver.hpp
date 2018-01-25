#ifndef guard_RECEIVER_HPP_
#define guard_RECEIVER_HPP_

#include "message_queue.hpp"
#include "sender.hpp"
#include "dispatcher.hpp"


namespace messaging
{

class receiver
{
    queue q;    // receiver owns the queue
public:
    operator sender()
    {
        return sender(&q);
    }

    dispatcher wait()   // waiting for a queue creates a dispatcher
    {
        return dispatcher(&q);
    }
};

} // namespace messaging


#endif /* guard_RECEIVER_HPP_ */
