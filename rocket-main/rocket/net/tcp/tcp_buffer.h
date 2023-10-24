/*
 * @Description: tcp数据缓冲池
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-16 23:13:36
 */
#ifndef ROCKET_NET_TCP_TCP_BUFFER_H
#define ROCKET_NET_TCP_TCP_BUFFER_H

#include <vector>
#include <memory>

namespace rocket
{

    class TcpBuffer
    {

    public:
        typedef std::shared_ptr<TcpBuffer> s_ptr;
        // 构造
        TcpBuffer(int size);

        ~TcpBuffer();

        // 返回可读字节数
        int readAble();

        // 返回可写的字节数
        int writeAble();

        //返回读索引
        int readIndex();

        //返回写索引
        int writeIndex();

        //写到缓冲区里
        void writeToBuffer(const char *buf, int size);

        //从缓冲区中读
        void readFromBuffer(std::vector<char> &re, int size);

        //重定义buffer长度
        void resizeBuffer(int new_size);
        //就是向前移动buffer，把空位取出
        void adjustBuffer();
        //手动调整index的值
        void moveReadIndex(int size);
        void moveWriteIndex(int size);

    private:
        int m_read_index {0};//读取位置
        int m_write_index {0};//写入位置
        int m_size {0}; //buffer大小

    public:
        std::vector<char> m_buffer;
    };

}

#endif