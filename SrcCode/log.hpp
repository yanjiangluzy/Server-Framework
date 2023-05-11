#pragma once
#include <string>
#include <stdint.h>
#include <memory>

// 日志系统
// 主要有三部分：日志器，日志格式以及日志的级别
// 日志系统可以参考log4cpp
namespace zy
{
    class LogEvent
    {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent();

    private:
        const char *m_file = nullptr; // 文件名
        int32_t m_line = 0;           // 行号
        uint32_t m_elapse;            // 程序运行到现在的毫秒数
        uint32_t m_threadId = 0;      // 线程id
        uint32_t m_fiberId = 0;       // 协程id
        uint64_t m_time = 0;          // 时间戳
        std::string m_content;        // 具体内容
    };

    class LogLevel
    {
    public:
        enum Level
        {
            DEBUG = 1,
            INFO = 2,
            WARING = 3,
            ERROR = 4,
            FATAL = 5
        };
    };

    // 日志格式器，可以由用户指定不同的格式进行不同的输出
    class LogFormatter
    {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        std::string format(LogEvent::ptr event); // 对其进行格式化
    private:
    };

    // 日志输出地，方便进行不同目的地的输出
    class LogAppender
    {
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        void log(LogLevel level, LogEvent::ptr event);
        virtual ~LogAppender(){};
    private:
        LogLevel::Level m_level;
    };

    // 日志器
    class Logger
    {
    public:
        typedef std::shared_ptr<Logger> ptr;
        Logger(std::string name = "root"){};

        void log(LogLevel::Level level, LogEvent::ptr Event);

    private:
        std::string m_name;      // 一个日志器的名字，根据这个名字来区分不同的日志器
        LogLevel::Level m_level; // 一个日志器，对应输出的日志等级
        LogAppender::ptr;
    };

}