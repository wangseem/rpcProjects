/*
 * @Description: 这就是封装了struct epoll_event 的一个类
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-25 23:28:33
 */
/*
这个类就相当于
struct epoll_event {
        uint32_t     events;      //Epoll events  这个就是那个enum
        epoll_data_t data;        // User data variable 这个就相当于这个联合体的fd类型
};
typedef union epoll_data {
        void        *ptr;
        int          fd;        // 通常情况下使用这个成员, 和epoll_ctl的第三个参数相同即可
        uint32_t     u32;
        uint64_t     u64;
} epoll_data_t;
*/
#ifndef ROCKET_NET_FDEVENT_H
#define ROCKET_NET_FDEVENT_H

#include <functional>
#include <sys/epoll.h>

namespace rocket
{
    class FdEvent
    {
    public:
        enum TriggerEvent
        {
            IN_EVENT = EPOLLIN,
            OUT_EVENT = EPOLLOUT,
            ERROR_EVENT = EPOLLERR,
        };

        FdEvent(int fd);

        FdEvent();

        ~FdEvent();

        void setNonBlock(); // 设置fd非阻塞让我caicai fcntl(F_GETFL)

        std::function<void()> handler(TriggerEvent event_type);

        void listen(TriggerEvent event_type, std::function<void()> callback, std::function<void()> error_callback = nullptr);

        // 取消监听
        void cancle(TriggerEvent event_type);

        int getFd() const
        {
            return m_fd;
        }

        epoll_event getEpollEvent()
        {
            return m_listen_events;
        }

    protected:
        int m_fd{-1};

        epoll_event m_listen_events;

        std::function<void()> m_read_callback{nullptr}; //读事件的callback
        std::function<void()> m_write_callback{nullptr}; //写事件的callback
        std::function<void()> m_error_callback{nullptr}; //当epoll返回epollerr的时候的callback
    };

}

#endif