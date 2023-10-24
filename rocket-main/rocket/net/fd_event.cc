/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-17 00:40:05
 */
#include <string.h>
#include <fcntl.h>
#include "rocket/net/fd_event.h"
#include "rocket/common/log.h"

namespace rocket
{

    FdEvent::FdEvent(int fd) : m_fd(fd)
    {
        memset(&m_listen_events, 0, sizeof(m_listen_events));
    }

    FdEvent::FdEvent()
    {
        memset(&m_listen_events, 0, sizeof(m_listen_events));
    }

    FdEvent::~FdEvent()
    {
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-28 16:33:51
     * @Description: handler处理函数
     * @param {TriggerEvent} event
     * @return {*}
     */    
    std::function<void()> FdEvent::handler(TriggerEvent event)
    {
        if (event == TriggerEvent::IN_EVENT)
        {
            return m_read_callback;
        }
        else if (event == TriggerEvent::OUT_EVENT)
        {
            return m_write_callback;
        }
        else if (event == TriggerEvent::ERROR_EVENT)
        {
            return m_error_callback;
        }
        return nullptr;
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-28 16:32:51
     * @Description: 就是根据传入的event 去判断调用那个回调函数；
     * @param {TriggerEvent} event_type
     * @param {function<void()>} callback
     * @param {function<void()>} error_callback
     * @return {*}
     */
    void FdEvent::listen(TriggerEvent event_type, std::function<void()> callback, std::function<void()> error_callback /*= nullptr*/)
    {
        if (event_type == TriggerEvent::IN_EVENT)
        {
            m_listen_events.events |= EPOLLIN;
            m_read_callback = callback;
        }
        else
        {
            m_listen_events.events |= EPOLLOUT;
            m_write_callback = callback;
        }

        if (m_error_callback == nullptr)
        {
            m_error_callback = error_callback;
        }
        else
        {
            m_error_callback = nullptr;
        }

        m_listen_events.data.ptr = this;
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-17 00:14:13
     * @Description: 取消监听fd
     * @param {TriggerEvent} event_type
     * @return {*}
     */
    void FdEvent::cancle(TriggerEvent event_type)
    {
        if (event_type == TriggerEvent::IN_EVENT)
        {
            m_listen_events.events &= (~EPOLLIN);
        }
        else
        {
            m_listen_events.events &= (~EPOLLOUT);
        }
    }

    // 设置非阻塞
    void FdEvent::setNonBlock()
    {

        int flag = fcntl(m_fd, F_GETFL, 0);
        if (flag & O_NONBLOCK)
        {
            return;
        }

        fcntl(m_fd, F_SETFL, flag | O_NONBLOCK);
    }

}