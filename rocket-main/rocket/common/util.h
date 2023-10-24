/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: 
 * @LastEditTime: 2023-09-16 23:16:14
 */
#ifndef ROCKET_COMMON_UTIL_H
#define ROCKET_COMMON_UTIL_H

#include <sys/types.h>
#include <unistd.h>

namespace rocket
{

    pid_t getPid();

    pid_t getThreadId();

    int64_t getNowMs();

    int32_t getInt32FromNetByte(const char *buf);

}

#endif