/*
 * @Description: msg_id生成器随机的获取一个字符序列
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-25 16:45:09
 */
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "rocket/common/msg_id_util.h"
#include "rocket/common/log.h"

namespace rocket
{

    static int g_msg_id_length = 20; //msg——id默认
    static int g_random_fd = -1;     //随机fd

    static thread_local std::string t_msg_id_no; //线程局部变量//当前id
    static thread_local std::string t_max_msg_id_no; //最大id

    //获取信息id生成msgid
    std::string MsgIDUtil::GenMsgID()
    {
        if (t_msg_id_no.empty() || t_msg_id_no == t_max_msg_id_no)//如果是空的
        {
            if (g_random_fd == -1)
            {
                g_random_fd = open("/dev/urandom", O_RDONLY);//构建一个随机数的fd
            }
            std::string res(g_msg_id_length, 0);
            if ((read(g_random_fd, &res[0], g_msg_id_length)) != g_msg_id_length)//从fd的读取lenth 到res里
            {
                ERRORLOG("read form /dev/urandom error");
                return "";
            }
            for (int i = 0; i < g_msg_id_length; ++i)
            {
                uint8_t x = ((uint8_t)(res[i])) % 10;
                res[i] = x + '0';
                t_max_msg_id_no += "9";
            }
            t_msg_id_no = res;
        }
        else//如果当前这个有加1就好了
        {
            size_t i = t_msg_id_no.length() - 1;
            while (t_msg_id_no[i] == '9' && i >= 0)
            {
                i--;
            }
            if (i >= 0)
            {
                t_msg_id_no[i] += 1;
                for (size_t j = i + 1; j < t_msg_id_no.length(); ++j)
                {
                    t_msg_id_no[j] = '0';
                }
            }
        }

        return t_msg_id_no;//长度是20
    }

}
