/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-19 16:21:34
 */
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <string.h>
#include <arpa/inet.h>
#include "rocket/common/util.h"

namespace rocket
{

    static int g_pid = 0;

    static thread_local int t_thread_id = 0;
    /**
     * @Author: kevin
     * @Date: 2023-09-19 12:58:04
     * //获取进程id
     * @Description: 
     * @return {*}
     */    
    pid_t getPid()
    {
        if (g_pid != 0)
        {
            return g_pid;
        }
        return getpid();
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-19 12:57:37
     * @Description:获取线程id 
     * @return {*}
     */    
    pid_t getThreadId()
    {
        if (t_thread_id != 0)
        {
            return t_thread_id;
        }
        return syscall(SYS_gettid);
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-19 12:57:21
     * @Description: 获取时间信息
     * @return {*}
     */    
    int64_t getNowMs()
    {
        timeval val;
        gettimeofday(&val, NULL);

        return val.tv_sec * 1000 + val.tv_usec / 1000;
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-19 12:29:29
     * @Description: 将buffer里的数从网络字节序转化到主机字节序
     * @param {char} *buf
     * @return {*}
     */    
    int32_t getInt32FromNetByte(const char *buf)
    {
        int32_t re;
        memcpy(&re, buf, sizeof(re));
        return ntohl(re);
        /*ntohl()将一个无符号长整形数从网络字节顺序转换为主机字节顺序，*/
        /*ntohl()返回一个以主机字节顺序表达的数。*/
    }
    //explain：字节序分为大端字节序和小端字节序
    /*  大端字节序是指一个整数的高位字节（32-31bit）存储在内存的低地址处，低位字节（0-7bit）存储在内存的高地址处。
        小端字节序是指一个整数的高位字节（32-31bit）存储在内存的高地址处，低位字节（0-7bit）存储在内存的低地址处。
        现代PC大多采用小端字节序，所以小端字节序又被称为主机字节序。
        大端字节序也称为网络字节序*/


}