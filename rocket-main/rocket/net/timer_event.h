/*
 * @Description:  定时器事件，里面有定时器任务的各种信息，每个event 就是一个定时器任务
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-30 14:51:56
 */
#ifndef ROCKET_NET_TIMEREVENT
#define ROCKET_NET_TIMEREVENT

#include <functional>
#include <memory>

namespace rocket
{

    class TimerEvent
    {

    public:
        typedef std::shared_ptr<TimerEvent> s_ptr;

        TimerEvent(int interval, bool is_repeated, std::function<void()> cb);

        int64_t getArriveTime() const
        {
            return m_arrive_time;
        }

        void setCancled(bool value)
        {
            m_is_cancled = value;
        }

        bool isCancled()
        {
            return m_is_cancled;
        }

        bool isRepeated()
        {
            return m_is_repeated;
        }

        std::function<void()> getCallBack()
        {
            return m_task;
        }

        void resetArriveTime();

    private:
        int64_t m_arrive_time; // ms
        int64_t m_interval;    // ms
        bool m_is_repeated{false};
        bool m_is_cancled{false};

        std::function<void()> m_task; //回调函数
    };

}

#endif