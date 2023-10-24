/*
 * @Description: 日志系统
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-28 15:24:14
 */
// 特点每个类都有一个智能指针类型怎么说，智能指针就是一个模板类
#ifndef ROCKET_COMMON_LOG_H
#define ROCKET_COMMON_LOG_H

#include <string>
#include <queue>
#include <memory>
#include <semaphore.h>

#include "rocket/common/config.h"
#include "rocket/common/mutex.h"
#include "rocket/net/timer_event.h"

namespace rocket
{

/**
 * @Author: kevin
 * @Date: 2023-09-04 19:16:38
 * @Description: 日志打印模块
 * @return {*}
 */
/************************************************************************************************************************/
#define DEBUGLOG(str, ...)                                                                                                                                                                                                         \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() && rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Debug)                                                                                                     \
    {                                                                                                                                                                                                                              \
        rocket::Logger::GetGlobalLogger()->pushLog(rocket::LogEvent(rocket::LogLevel::Debug).toString() + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + "\n"); \
    }

#define INFOLOG(str, ...)                                                                                                                                                                                                         \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Info)                                                                                                                                                         \
    {                                                                                                                                                                                                                             \
        rocket::Logger::GetGlobalLogger()->pushLog(rocket::LogEvent(rocket::LogLevel::Info).toString() + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + "\n"); \
    }

#define ERRORLOG(str, ...)                                                                                                                                                                                                         \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Error)                                                                                                                                                         \
    {                                                                                                                                                                                                                              \
        rocket::Logger::GetGlobalLogger()->pushLog(rocket::LogEvent(rocket::LogLevel::Error).toString() + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + "\n"); \
    }

#define APPDEBUGLOG(str, ...)                                                                                                                                                                                                         \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Debug)                                                                                                                                                            \
    {                                                                                                                                                                                                                                 \
        rocket::Logger::GetGlobalLogger()->pushAppLog(rocket::LogEvent(rocket::LogLevel::Debug).toString() + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + "\n"); \
    }

#define APPINFOLOG(str, ...)                                                                                                                                                                                                         \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Info)                                                                                                                                                            \
    {                                                                                                                                                                                                                                \
        rocket::Logger::GetGlobalLogger()->pushAppLog(rocket::LogEvent(rocket::LogLevel::Info).toString() + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + "\n"); \
    }

#define APPERRORLOG(str, ...)                                                                                                                                                                                                         \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Error)                                                                                                                                                            \
    {                                                                                                                                                                                                                                 \
        rocket::Logger::GetGlobalLogger()->pushAppLog(rocket::LogEvent(rocket::LogLevel::Error).toString() + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + "\n"); \
    }                                                                                                                                                                                                                                 \
    /************************************************************************************************************************/
    
    /**
     * @Author: kevin
     * @Date: 2023-09-04 19:09:41
     * @Description:  格式化字符串用于日志打印
     * @return {*}
     */
    template <typename... Args>
    std::string formatString(const char *str, Args &&...args)
    {

        int size = snprintf(nullptr, 0, str, args...);

        std::string result;
        if (size > 0)
        {
            result.resize(size);
            snprintf(&result[0], size + 1, str, args...);
        }

        return result;
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-04 19:17:27
     * @Description: 全局日志及级别
     * @return {*}
     */
    enum LogLevel
    {
        Unknown = 0,
        Debug = 1,
        Info = 2,
        Error = 3
    };

    /**
     * @Author: kevin
     * @Date: 2023-09-04 19:22:04
     * @Description: 日志转换
     * @param {LogLevel} level
     * @return {*}
     */
    std::string LogLevelToString(LogLevel level);

    /**
     * @Author: kevin
     * @Date: 2023-09-04 19:22:26
     * @Description: 日志转换
     * @param {string&} log_level
     * @return {*}
     */
    LogLevel StringToLogLevel(const std::string &log_level);

    /**
     * @Author: kevin
     * @Date: 2023-09-04 19:14:24
     * @Description: //异步日志的类不明白为什么这样子
     * @return {*}
     */
    class AsyncLogger
    {

    public:
        typedef std::shared_ptr<AsyncLogger> s_ptr; // 智能指针指向日志打印器

        AsyncLogger(const std::string &file_name, const std::string &file_path, int max_size);

        void stop();

        // 刷新到磁盘
        void flush();

        void pushLogBuffer(std::vector<std::string> &vec);

    public:
        static void *Loop(void *);

    public:
        pthread_t m_thread;

    private:
        // m_file_path/m_file_name_yyyymmdd.0

        std::queue<std::vector<std::string>> m_buffer;
        std::string m_file_name; // 日志输出文件名字
        std::string m_file_path; // 日志输出路径
        std::string m_date;      // 当前打印日志的文件日期

        int m_max_file_size{0}; // 日志单个文件最大大小, 单位为字节

        pthread_cond_t m_condtion; // 条件变量

        FILE *m_file_hanlder{NULL}; // 当前打开的日志文件句柄

        int m_no{0}; // 日志文件序号

        bool m_reopen_flag{false};
        bool m_stop_flag{false};
        sem_t m_sempahore;
        Mutex m_mutex;
    };
    /**
     * @Author: kevin
     * @Date: 2023-09-04 19:13:44
     * @Description: 就是打印日志的一个jb打印器
     * @return {*}
     */
    class Logger
    {
    public:
        typedef std::shared_ptr<Logger> s_ptr;

        Logger(LogLevel level, int type = 1);

        void pushLog(const std::string &msg);//往队列里push日志

        void pushAppLog(const std::string &msg);

        void init();

        void log();

        LogLevel getLogLevel() const
        {
            return m_set_level;
        }

        AsyncLogger::s_ptr getAsyncAppLopger()
        {
            return m_asnyc_app_logger;
        }

        AsyncLogger::s_ptr getAsyncLopger()
        {
            return m_asnyc_logger;
        }

        void syncLoop();

        void flush();

    public:
        static Logger *GetGlobalLogger();

        static void InitGlobalLogger(int type = 1);

    private:
        LogLevel m_set_level;
        std::vector<std::string> m_buffer;

        std::vector<std::string> m_app_buffer;

        Mutex m_mutex;

        Mutex m_app_mutex;

        // m_file_path/m_file_name_yyyymmdd.1

        std::string m_file_name; // 日志输出文件名字
        std::string m_file_path; // 日志输出路径
        int m_max_file_size{0};  // 日志单个文件最大大小

        AsyncLogger::s_ptr m_asnyc_logger; //异步日志智能指针

        AsyncLogger::s_ptr m_asnyc_app_logger;//异步日志智能指针

        TimerEvent::s_ptr m_timer_event;

        int m_type{0};
    };

    /**
     * @Author: kevin
     * @Date: 2023-09-04 19:13:20
     * @Description: 日志事件人话就是日志
     * @return {*}
     */
    class LogEvent
    {
    public:
        LogEvent(LogLevel level) : m_level(level) {}
        /**
         * @Author: kevin
         * @Date: 2023-09-04 19:29:34
         * @Description: 获取日志文件名
         * @return {*}
         */
        std::string getFileName() const
        {
            return m_file_name;    
        }
        
        /**
         * @Author: kevin
         * @Date: 2023-09-04 19:29:52
         * @Description: 获取日志等级
         * @return {*}
         */        
        LogLevel getLogLevel() const
        {
            return m_level;
        }

        std::string toString();

    private:
        std::string m_file_name; // 文件名
        int32_t m_file_line;     // 行号
        int32_t m_pid;           // 进程号
        int32_t m_thread_id;     // 线程号

        LogLevel m_level; // 日志级别
    };

}

#endif