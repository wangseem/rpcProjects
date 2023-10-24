/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-16 23:14:50
 */
#include "rocket/net/io_thread_group.h"
#include "rocket/common/log.h"

namespace rocket
{
    /**
     * @Author: kevin
     * @Date: 2023-09-14 15:36:35
     * @Description: 创建iO线程
     * @param {int} size
     * @return {*}
     */
    IOThreadGroup::IOThreadGroup(int size) : m_size(size)
    {
        m_io_thread_groups.resize(size);
        for (size_t i = 0; (int)i < size; ++i)
        {
            m_io_thread_groups[i] = new IOThread();
        }
    }

    IOThreadGroup::~IOThreadGroup()
    {
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-14 15:36:57
     * @Description: 逐个开启iO线程
     * @return {*}
     */    
    void IOThreadGroup::start()
    {
        for (size_t i = 0; i < m_io_thread_groups.size(); ++i)
        {
            m_io_thread_groups[i]->start();
        }
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-14 15:37:20
     * @Description: 逐个开启join线程
     * @return {*}
     */    
    void IOThreadGroup::join()
    {
        for (size_t i = 0; i < m_io_thread_groups.size(); ++i)
        {
            m_io_thread_groups[i]->join();
        }
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-14 15:37:41
     * @Description: 获取index表示的io线程
     * @return {*}
     */    
    IOThread *IOThreadGroup::getIOThread()
    {
        if (m_index == (int)m_io_thread_groups.size() || m_index == -1)
        {
            m_index = 0;
        }
        return m_io_thread_groups[m_index++];
    }

}