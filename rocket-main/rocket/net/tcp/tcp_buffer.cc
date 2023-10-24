#include <memory>
#include <string.h>
#include "rocket/common/log.h"
#include "rocket/net/tcp/tcp_buffer.h"

namespace rocket
{

    TcpBuffer::TcpBuffer(int size) : m_size(size)
    {
        m_buffer.resize(size);
    }

    TcpBuffer::~TcpBuffer()
    {
    }

    // 返回可读字节数
    int TcpBuffer::readAble()
    {
        return m_write_index - m_read_index;
    }

    // 返回可写的字节数
    int TcpBuffer::writeAble()
    {
        return m_buffer.size() - m_write_index;
    }

    int TcpBuffer::readIndex()
    {
        return m_read_index;
    }

    int TcpBuffer::writeIndex()
    {
        return m_write_index;
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-14 16:18:37
     * @Description: 往buffer里写
     * @param {char} *buf
     * @param {int} size
     * @return {*}
     */
    void TcpBuffer::writeToBuffer(const char *buf, int size)
    {
        if (size > writeAble())
        {
            // 调整 buffer 的大小，扩容
            int new_size = (int)(1.5 * (m_write_index + size));
            resizeBuffer(new_size);
        }
        memcpy(&m_buffer[m_write_index], buf, size);
        m_write_index += size;
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-14 16:16:25
     * @Description: 从re开始读size个字节
     * @param {vector<char>} &re
     * @param {int} size
     * @return {*}
     */    
    void TcpBuffer::readFromBuffer(std::vector<char> &re, int size)
    {
        if (readAble() == 0)
        {
            return;
        }
        int read_size = readAble() > size ? size : readAble();

        std::vector<char> tmp(read_size);
        memcpy(&tmp[0], &m_buffer[m_read_index], read_size);

        re.swap(tmp);
        m_read_index += read_size;

        adjustBuffer();
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-14 16:27:45
     * @Description: 重新调整buffer大小
     * @param {int} new_size
     * @return {*}
     */    
    void TcpBuffer::resizeBuffer(int new_size)
    {
        std::vector<char> tmp(new_size);
        int count = std::min(new_size, readAble());

        memcpy(&tmp[0], &m_buffer[m_read_index], count);
        m_buffer.swap(tmp);

        m_read_index = 0;
        m_write_index = m_read_index + count;
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-14 16:25:21
     * @Description: 就是向前移动buffer，把空位取出
     * @return {*}
     */
    void TcpBuffer::adjustBuffer()
    {
        if (m_read_index < int(m_buffer.size() / 3))
        {
            return;
        }
        std::vector<char> buffer(m_buffer.size());
        int count = readAble();

        memcpy(&buffer[0], &m_buffer[m_read_index], count);
        m_buffer.swap(buffer);
        m_read_index = 0;
        m_write_index = m_read_index + count;

        buffer.clear();
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-19 13:04:10
     * @Description: 移动readindex
     * @param {int} size
     * @return {*}
     */     
    void TcpBuffer::moveReadIndex(int size)
    {
        size_t j = m_read_index + size;
        if (j >= m_buffer.size())
        {
            ERRORLOG("moveReadIndex error, invalid size %d, old_read_index %d, buffer size %d", size, m_read_index, m_buffer.size());
            return;
        }
        m_read_index = j;
        adjustBuffer();
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-19 13:04:25
     * @Description: 移动写索引位置
     * @param {int} size
     * @return {*}
     */    
    void TcpBuffer::moveWriteIndex(int size)
    {
        size_t j = m_write_index + size;
        if (j >= m_buffer.size())
        {
            ERRORLOG("moveWriteIndex error, invalid size %d, old_read_index %d, buffer size %d", size, m_read_index, m_buffer.size());
            return;
        }
        m_write_index = j;
        adjustBuffer();
    }

}