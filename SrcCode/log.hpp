#pragma once
#include <string>
#include <stdint.h>
#include <memory>

// 日志系统
// 主要有三部分：日志器，日志格式以及日志的级别
namespace zy
{
    class LogEvent
    {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent();
    private:
        const char* m_file = nullptr; // 文件名
        int32_t m_line = 0;           // 行号
        uint32_t m_elapse;            // 程序运行到现在的毫秒数
        uint32_t m_threadId = 0;      // 线程id
        uint32_t m_fiberId = 0;       // 协程id
        uint64_t m_time = 0;          // 时间戳
        std::string m_content;        // 具体内容
    };

    class LogLevel
    {
        enum
        {
            DEBUG = 1,
            INFO = 2,
            WARING = 3,
            ERROR = 4,
            FATAL = 5
        };
    };


    // 日志器
    class Logger
    {
        public:
            typedef std::shared_ptr<Logger> ptr;
            Logger(std::string name = "root") {};
        private:

    };

    // 日志输出地，方便进行不同目的地的输出
    class LogAppender
    {
    public:
        virtual ~LogAppender(){};
    };
}