/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: 
 * @LastEditTime: 2023-09-16 23:14:01
 */
/*
 * @Description: 创建一个新线程，她会帮我们执行以下操作
    1. 创建一个新线程（pthread_create）
    2. 在新线程里面 创建一个 EventLoop，完成初始化
    3. 开启 loop
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-14 15:16:04
 */

#ifndef ROCKET_NET_IO_THREAD_H
#define ROCKET_NET_IO_THREAD_H

#include <pthread.h>
#include <semaphore.h>
#include "rocket/net/eventloop.h"

namespace rocket
{

    class IOThread
    {
    public:
        IOThread();

        ~IOThread();

        EventLoop *getEventLoop();

        void start();//唤醒main函数的loop函数主动唤醒

        void join(); 

    public:
        static void *Main(void *arg);//静态函数//新线程入口，arg传入是ioThread的this指针

    private:
        pid_t m_thread_id{-1}; // 线程号
        pthread_t m_thread{0}; // 线程句柄

        EventLoop *m_event_loop{NULL}; // 当前 io 线程的 loop 对象

        sem_t m_init_semaphore;//构造函数创建线程时让他等待子线程创建完eventloop

        sem_t m_start_semaphore; //控制启动的信号量
    };

}

#endif