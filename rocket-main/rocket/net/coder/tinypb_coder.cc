#include <vector>
#include <string.h>
#include <arpa/inet.h>
#include "rocket/net/coder/tinypb_coder.h"
#include "rocket/net/coder/tinypb_protocol.h"
#include "rocket/common/util.h"
#include "rocket/common/log.h"

namespace rocket
{

    /**
     * @Author: kevin
     * @Date: 2023-09-19 12:06:18
     * // 将 message 对象转化为字节流，写入到 buffer
     * @Description: 
     * @return {*}
     */    
    void TinyPBCoder::encode(std::vector<AbstractProtocol::s_ptr> &messages, TcpBuffer::s_ptr out_buffer)
    {
        for (auto &i : messages)//数组每个元素转成TinyPBprotocol
        {
            std::shared_ptr<TinyPBProtocol> msg = std::dynamic_pointer_cast<TinyPBProtocol>(i);//对每一个数据包进行解析//将父类的数据转换为子类类型；
            int len = 0;
            //开始字节序转换
            const char *buf = encodeTinyPB(msg, len);//将单个的包转化为字节流
            if (buf != NULL && len != 0)
            {
                out_buffer->writeToBuffer(buf, len);
            }

            //释放buffer内存资源
            if (buf)
            {
                free((void *)buf);
                buf = NULL;
            }
        }
    }

    // {char开始符 1---int32_t 整包长---int32_tMSG_ID长度---int32_tMSG_ID---int32_t方法名长度---string方法名---int32_t错误码---int32_t错误信息长度---string错误信息---stringprotobuf序列化数据---int32_t校验码---char结束符 }
    /**
     * @Author: kevin
     * @Date: 2023-09-19 12:24:03
    // 将 buffer 里面的字节流转换为 message 对象
     * @Description: 
     * @return {*}
     */
    void TinyPBCoder::decode(std::vector<AbstractProtocol::s_ptr> &out_messages, TcpBuffer::s_ptr buffer)
    {
        while (1)
        {
            // 遍历 buffer，找到 PB_START，找到之后，解析出整包的长度。然后得到结束符的位置，判断是否为 PB_END
            std::vector<char> tmp = buffer->m_buffer;//复制buffer
            int start_index = buffer->readIndex();//开始索引
            int end_index = -1;//结束索引 

            int pk_len = 0;
            bool parse_success = false;
            int i = 0;
            for (i = start_index; i < buffer->writeIndex(); ++i)//挨个读buffer
            {
                if (tmp[i] == TinyPBProtocol::PB_START) //一个字节一个字节的比较//找到头位置
                {
                    // 读下去四个字节。由于是网络字节序，需要转为主机字节序
                    if (i + 1 < buffer->writeIndex())//如果下一个字节可读
                    {
                        pk_len = getInt32FromNetByte(&tmp[i + 1]);//转化成主机字节序
                        DEBUGLOG("get pk_len = %d", pk_len);

                        // 结束符的索引
                        int j = i + pk_len - 1;
                        if (j >= buffer->writeIndex())//如果结束符的位置大于buffer的末尾就进入下一个循环
                        {
                            continue;
                        }
                        if (tmp[j] == TinyPBProtocol::PB_END)
                        {
                            start_index = i;
                            end_index = j;
                            parse_success = true;
                            break;
                        }
                    }
                }
            }

            if (i >= buffer->writeIndex())
            {
                DEBUGLOG("decode end, read all buffer data");
                return;
            }

            if (parse_success)//如果解析成功
            {
                buffer->moveReadIndex(end_index - start_index + 1);//移动包个长度
                std::shared_ptr<TinyPBProtocol> message = std::make_shared<TinyPBProtocol>();
                message->m_pk_len = pk_len;

                int msg_id_len_index = start_index + sizeof(char) + sizeof(message->m_pk_len);
                if (msg_id_len_index >= end_index)//如果id索引大于结束位置说明解析错误
                {
                    message->parse_success = false;
                    ERRORLOG("parse error, msg_id_len_index[%d] >= end_index[%d]", msg_id_len_index, end_index);
                    continue;
                }

                //获取msg_id长度
                message->m_msg_id_len = getInt32FromNetByte(&tmp[msg_id_len_index]);
                DEBUGLOG("parse msg_id_len=%d", message->m_msg_id_len);

                //获取msg_id
                int msg_id_index = msg_id_len_index + sizeof(message->m_msg_id_len);
                char msg_id[100] = {0};
                memcpy(&msg_id[0], &tmp[msg_id_index], message->m_msg_id_len);
                message->m_msg_id = std::string(msg_id);
                DEBUGLOG("parse msg_id=%s", message->m_msg_id.c_str());

                //获取方法名长度
                int method_name_len_index = msg_id_index + message->m_msg_id_len;
                if (method_name_len_index >= end_index)
                {
                    message->parse_success = false;
                    ERRORLOG("parse error, method_name_len_index[%d] >= end_index[%d]", method_name_len_index, end_index);
                    continue;
                }
                message->m_method_name_len = getInt32FromNetByte(&tmp[method_name_len_index]);

                //获取方法名
                int method_name_index = method_name_len_index + sizeof(message->m_method_name_len);
                char method_name[512] = {0};
                memcpy(&method_name[0], &tmp[method_name_index], message->m_method_name_len);
                message->m_method_name = std::string(method_name);
                DEBUGLOG("parse method_name=%s", message->m_method_name.c_str());

                //获取错位码位置
                int err_code_index = method_name_index + message->m_method_name_len;
                if (err_code_index >= end_index)//判断位置
                {
                    message->parse_success = false;
                    ERRORLOG("parse error, err_code_index[%d] >= end_index[%d]", err_code_index, end_index);
                    continue;
                }
                //获取错误码；
                message->m_err_code = getInt32FromNetByte(&tmp[err_code_index]);

                //获取错误信息长度
                int error_info_len_index = err_code_index + sizeof(message->m_err_code);
                if (error_info_len_index >= end_index)
                {
                    message->parse_success = false;
                    ERRORLOG("parse error, error_info_len_index[%d] >= end_index[%d]", error_info_len_index, end_index);
                    continue;
                }
                message->m_err_info_len = getInt32FromNetByte(&tmp[error_info_len_index]);

                //获取错误信息
                int err_info_index = error_info_len_index + sizeof(message->m_err_info_len);
                char error_info[512] = {0};
                memcpy(&error_info[0], &tmp[err_info_index], message->m_err_info_len);
                message->m_err_info = std::string(error_info);
                DEBUGLOG("parse error_info=%s", message->m_err_info.c_str());

                //获取protobuf 数据长度
                int pb_data_len = message->m_pk_len - message->m_method_name_len - message->m_msg_id_len - message->m_err_info_len - 2 - 24;

                //获取protobuf数据
                int pd_data_index = err_info_index + message->m_err_info_len;
                message->m_pb_data = std::string(&tmp[pd_data_index], pb_data_len);

                // 这里校验和去解析
                //校验和和解析
                message->parse_success = true;
                out_messages.push_back(message);
            }
        }
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-19 12:11:36
     * @Description: 将msg转换成字节流
     * @param {shared_ptr<TinyPBProtocol>} message
     * @param {int} &len
     * @return {*}返回的是字符串哦const char *;
     */    
    // TinyPBProtocol就是那个结构体，协议结构体
    const char *TinyPBCoder::encodeTinyPB(std::shared_ptr<TinyPBProtocol> message, int &len)
    {
        //如果msg_id是空的就给她赋值123456789
        if (message->m_msg_id.empty())
        {
            message->m_msg_id = "123456789";//随便给的啊
        }
        DEBUGLOG("msg_id = %s", message->m_msg_id.c_str());

        //2 是两个char 24 是6个int_32
        //包长度
        int pk_len = 2 + 24 + message->m_msg_id.length() + message->m_method_name.length() + message->m_err_info.length() + message->m_pb_data.length();
        DEBUGLOG("pk_len = %", pk_len);

        //分配内存
        char *buf = reinterpret_cast<char *>(malloc(pk_len));
        char *tmp = buf;
        //先添加开始符
        *tmp = TinyPBProtocol::PB_START;
        tmp++;//tmp移动

        //加上包长度
        int32_t pk_len_net = htonl(pk_len);//主机序列转到网络字节序
        memcpy(tmp, &pk_len_net, sizeof(pk_len_net));
        tmp += sizeof(pk_len_net);

        //加上msg_id长度
        int msg_id_len = message->m_msg_id.length();
        int32_t msg_id_len_net = htonl(msg_id_len);
        memcpy(tmp, &msg_id_len_net, sizeof(msg_id_len_net));
        tmp += sizeof(msg_id_len_net);

        if (!message->m_msg_id.empty())//如果不是空的就默认添加一定的长度
        {
            memcpy(tmp, &(message->m_msg_id[0]), msg_id_len);
            tmp += msg_id_len;
        }

        //添加方法名长度//主机序转为网络字节序
        int method_name_len = message->m_method_name.length();
        int32_t method_name_len_net = htonl(method_name_len);
        memcpy(tmp, &method_name_len_net, sizeof(method_name_len_net));
        tmp += sizeof(method_name_len_net);

        //如果名字不是空的就添加固定长度
        if (!message->m_method_name.empty())
        {
            memcpy(tmp, &(message->m_method_name[0]), method_name_len);
            tmp += method_name_len;
        }

        //添加错误码
        int32_t err_code_net = htonl(message->m_err_code);
        memcpy(tmp, &err_code_net, sizeof(err_code_net));
        tmp += sizeof(err_code_net);

        //添加错误信息长度
        int err_info_len = message->m_err_info.length();
        int32_t err_info_len_net = htonl(err_info_len);
        memcpy(tmp, &err_info_len_net, sizeof(err_info_len_net));
        tmp += sizeof(err_info_len_net);

        //添加错误信息
        if (!message->m_err_info.empty())
        {
            memcpy(tmp, &(message->m_err_info[0]), err_info_len);
            tmp += err_info_len;
        }
        
        //添加protobuf数据
        if (!message->m_pb_data.empty())
        {
            memcpy(tmp, &(message->m_pb_data[0]), message->m_pb_data.length());
            tmp += message->m_pb_data.length();
        }

        //校验码采用0/1校验
        int32_t check_sum_net = htonl(1);
        memcpy(tmp, &check_sum_net, sizeof(check_sum_net));
        tmp += sizeof(check_sum_net);

        *tmp = TinyPBProtocol::PB_END;
        

        message->m_pk_len = pk_len;
        message->m_msg_id_len = msg_id_len;
        message->m_method_name_len = method_name_len;
        message->m_err_info_len = err_info_len;
        message->parse_success = true;
        len = pk_len;
        DEBUGLOG("encode message[%s] success", message->m_msg_id.c_str());
        return buf;
    }

}