#ifndef guard_SENDER_HPP_
#define guard_SENDER_HPP_

#include "message_queue.hpp"

namespace messaging
{

class sender
{
    queue* q;
public:
    sender() : q(nullptr) { }
    explicit sender(queue* q_) : q(q_) { }

    template<typename Message>
    void send(const Message& msg)
    {
        if(q){
            q->push(msg);
        }
    }
};

} // namespace messaging


#endif /* guard_SENDER_HPP_ */
