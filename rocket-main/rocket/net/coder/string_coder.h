/*
 * @Description: 编码解码类两个
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-18 16:29:30
 */
#ifndef ROCKERT_NET_STRING_CODER_H
#define ROCKERT_NET_STRING_CODER_H

#include "rocket/net/coder/abstract_coder.h"
#include "rocket/net/coder/abstract_protocol.h"

namespace rocket
{

    class StringProtocol : public AbstractProtocol
    {

    public:
        std::string info;
    };

    class StringCoder : public AbstractCoder//继承制抽象类AbstractCoder
    {
        
        /**
         * @Author: kevin
         * @Date: 2023-09-18 15:19:22
         * @Description:  将 message 对象转化为字节流，写入到 buffer
         * @param {vector<AbstractProtocol::s_ptr>} &messages
         * @param {s_ptr} out_buffer
         * @return {*}
         */        
        void encode(std::vector<AbstractProtocol::s_ptr> &messages, TcpBuffer::s_ptr out_buffer)
        {
            for (size_t i = 0; i < messages.size(); ++i)
            {
                std::shared_ptr<StringProtocol> msg = std::dynamic_pointer_cast<StringProtocol>(messages[i]);
                out_buffer->writeToBuffer(msg->info.c_str(), msg->info.length());
            }
        }
        /**
         * @Author: kevin
         * @Date: 2023-09-18 15:19:50
         * @Description: 将 buffer 里面的字节流转换为 message 对象
         * @param {vector<AbstractProtocol::s_ptr>} &out_messages
         * @param {s_ptr} buffer
         * @return {*}
         */        
        void decode(std::vector<AbstractProtocol::s_ptr> &out_messages, TcpBuffer::s_ptr buffer)
        {
            std::vector<char> re;
            buffer->readFromBuffer(re, buffer->readAble());//将数据从buffer中读取到，readAble是一个递归函数，他会把所有的数据都递归获取到re，所以要做一个拼接
            std::string info;
            //进行拼接
            for (size_t i = 0; i < re.size(); ++i)
            {
                info += re[i];
            }
            std::shared_ptr<StringProtocol> msg = std::make_shared<StringProtocol>();
            msg->info = info;
            msg->m_msg_id = "123456";//服务的端口号也就是rpc请求的端口号
            out_messages.push_back(msg);//把这个输出的放到out——message里面
        }
    };

}

#endif