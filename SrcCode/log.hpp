#ifndef _ZY_LOG_H
#define _ZY_LOG_H

#include <string>
#include <stdint.h>
#include <memory>

// 日志系统
// 主要有三部分：日志器，日志格式以及日志的级别
// 日志系统可以参考log4cpp
// 提供一个LoggerWrap 包裹日志器
namespace zy_log
{
    // 日志事件
    class LogEvent
    {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
                 const char *file, int32_t line,
                 uint32_t elapse, , uint32_t thread_id,
                 uint32_t fiber_id, uint64_t time,
                 const std::string &thread_name);

        // 需要定义函数用于获取日志事件的属性

        // 返回文件名
        const char *getFile() { return m_file; }

        // 返回行号
        int32_t getLine() { return m_line; }

        // 返回耗时
        uint32_t getElapse() { return m_elapse; }

        // 返回线程ID
        uint32_t getThreadId() { return m_threadId; }

        // 返回协程ID
        uint32_t getFiberId() { return m_fiberId; }

        // 返回时间
        uint64_t getTime() { return m_time; }

        // 返回线程名称
        std::string getThreadName() { return m_threadName; }

        // 返回日志内容流 --- 返回引用
        std::stringstream &getSs() { return m_ss; }

        // 返回日志器
        std::shared_ptr<Logger> getLogger() { return m_logger; }

        // 返回日志等级
        LogLevel::Level getLevel() { return m_level; }

        // 格式化写入日志内容
        void format(const char *fmt, ...);

        // 可变参数
        void format(const char *fmt, va_list al);

    private:
        /// 文件名
        const char *m_file = nullptr;
        /// 行号
        int32_t m_line = 0;
        /// 程序启动开始到现在的毫秒数
        uint32_t m_elapse = 0;
        /// 线程ID
        uint32_t m_threadId = 0;
        /// 协程ID
        uint32_t m_fiberId = 0;
        /// 时间戳
        uint64_t m_time = 0;
        /// 线程名称
        std::string m_threadName;
        /// 日志内容流
        std::stringstream m_ss;
        /// 日志器
        std::shared_ptr<Logger> m_logger;
        /// 日志等级
        LogLevel::Level m_level;
        ~LogEvent(){};
    };

    // 日志事件包装器
    class LogEventWrap
    {
    public:
        LogEventWrap() {}

        // 获取日志事件
        LogEvent::ptr getLogEvent() const { return m_logEvent; }

        // 获取日志内容流
        std::stringstream getSs();
        ~LogEventWrap();

    private:
        LogEvent::ptr m_logEvent;
    };

    // 日志格式器
    class LogFormatter
    {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;

        LogFormatter(const std::string pattern); // pattern指定输出的格式
    };

    // 日志器
    class Logger
    {
    };

    // 日志等级
    class LogLevel
    {
    };
}
#endif
