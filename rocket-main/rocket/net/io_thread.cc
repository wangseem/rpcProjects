
#include <pthread.h>
#include <assert.h>
#include "rocket/net/io_thread.h"
#include "rocket/common/log.h"
#include "rocket/common/util.h"

namespace rocket
{

    IOThread::IOThread()
    {

        int rt = sem_init(&m_init_semaphore, 0, 0);//初始化信号量 信号量没有封装
        assert(rt == 0);

        rt = sem_init(&m_start_semaphore, 0, 0);//初始化信号量 信号量没有封装
        assert(rt == 0);
        /*int pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg);
         参数:
        thread: 传出参数，是无符号长整形数，线程创建成功, 会将线程ID写入到这个指针指向的内存中
        attr: 线程的属性, 一般情况下使用默认属性即可, 写NULL
        start_routine: 函数指针，创建出的子线程的处理动作，也就是该函数在子线程中执行。
        arg: 作为实参传递到 start_routine 指针指向的函数内部*/
        pthread_create(&m_thread, NULL, &IOThread::Main, this); //创建io线程 回调函数是main

        // wait, 直到新线程执行完 Main 函数的前置
        sem_wait(&m_init_semaphore);

        DEBUGLOG("IOThread [%d] create success", m_thread_id);
    }

    IOThread::~IOThread()
    {

        m_event_loop->stop();//停止事件循环
        sem_destroy(&m_init_semaphore);//销毁信号量
        sem_destroy(&m_start_semaphore);

        pthread_join(m_thread, NULL);

        if (m_event_loop)
        {
            delete m_event_loop;
            m_event_loop = NULL;
        }
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-14 14:06:21
     * @Description: io线程的回调函数
     * @param {void} *arg
     * @return {*}
     */    
    void *IOThread::Main(void *arg)
    {
        IOThread *thread = static_cast<IOThread *>(arg);

        thread->m_event_loop = new EventLoop();
        thread->m_thread_id = getThreadId();
        //创建完上面的部分才能让主线程去走

        // 唤醒等待的线程（主线程）
        sem_post(&thread->m_init_semaphore);

        // 让IO 线程等待，直到我们主动的启动
        DEBUGLOG("IOThread %d created, wait start semaphore", thread->m_thread_id);
        sem_wait(&thread->m_start_semaphore);

        DEBUGLOG("IOThread %d start loop ", thread->m_thread_id);
        thread->m_event_loop->loop();

        DEBUGLOG("IOThread %d end loop ", thread->m_thread_id);
        return NULL;
    }
    //获取io事件循环
    EventLoop *IOThread::getEventLoop()
    {
        return m_event_loop;
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-14 14:57:33
     * @Description: 唤醒main函数的loop函数主动唤醒
     * @return {*}
     */
    void IOThread::start()
    {
        DEBUGLOG("Now invoke IOThread %d", m_thread_id);
        sem_post(&m_start_semaphore);
    }

    void IOThread::join()
    {
        pthread_join(m_thread, NULL);
    }

}