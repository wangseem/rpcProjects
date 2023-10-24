/*
 * @Description: iO线程组
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-16 23:14:51
 */

#ifndef ROCKET_NET_IO_THREAD_GROUP_H
#define ROCKET_NET_IO_THREAD_GROUP_H

#include <vector>
#include "rocket/common/log.h"
#include "rocket/net/io_thread.h"

namespace rocket
{

    class IOThreadGroup
    {

    public:
        IOThreadGroup(int size);//构造函数初始值

        ~IOThreadGroup();//析构

        void start();//控制loop循环开始

        void join();

        IOThread *getIOThread();//获取一个io线程

    private:
        int m_size{0}; //线程租大小
        std::vector<IOThread *> m_io_thread_groups; //io线程数组，里面是线程的指针

        int m_index{0};
    };

}

#endif