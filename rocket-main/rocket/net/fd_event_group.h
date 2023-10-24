/*
 * @Description: 封装fd_event
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-16 23:13:10
 */
#ifndef ROCKET_NET_FD_EVENT_GROUP_H
#define ROCKET_NET_FD_EVENT_GROUP_H

#include <vector>
#include "rocket/common/mutex.h"
#include "rocket/net/fd_event.h"

namespace rocket
{

    class FdEventGroup
    {

    public:
    //构造函数，fd_event数组大小
        FdEventGroup(int size);

        ~FdEventGroup();
        //获取fd_event
        FdEvent *getFdEvent(int fd);

    public:
        static FdEventGroup *GetFdEventGroup();

    private:
        int m_size{0};
        std::vector<FdEvent *> m_fd_group;//fd数组
        Mutex m_mutex;//多个线程同时调用要加锁
    };

}

#endif