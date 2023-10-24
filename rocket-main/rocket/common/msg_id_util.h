/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: 
 * @LastEditTime: 2023-09-14 14:25:56
 */
#ifndef ROCKET_COMMON_MSGID_UTIL_H
#define ROCKET_COMMON_MSGID_UTIL_H

#include <string>

namespace rocket
{

    class MsgIDUtil
    {

    public:
        static std::string GenMsgID();
    };

}

#endif