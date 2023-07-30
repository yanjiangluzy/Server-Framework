#ifndef __ZY_LOG_H__
#define __ZY_LOG_H__

#include <string>
#include <memory>
#include <iostream>
#include <vector>
#include <list>
#include <sstream>
#include <fstream>
#include <tuple>
#include <ctime>

namespace zy_log
{
    // 日志事件
    class LogEvent
    {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent();

        const char* getFile() const { return m_file; }
        int32_t getLine() const { return m_line; }
        uint32_t getElapse() const { return m_elapse; }
        uint32_t getThreadId() const { return m_threadId; }
        uint32_t getFiberId() const { return m_fiberId; }
        uint64_t getTime() const { return m_time; }
        std::string getContent() const { return m_content; }

    private:
        const char *m_file = nullptr; // 文件名
        int32_t m_line = 0;           // 行号
        uint32_t m_elapse = 0;        // 程序启动开始到现在的毫秒数
        uint32_t m_threadId = 0;      // 线程id
        uint32_t m_fiberId = 0;       // 协程id
        uint64_t m_time = 0;          // 时间戳
        std::string m_content;        // 正文

        LogLevel::Level m_level;      // 日志等级
    };

    // 日志等级
    class LogLevel
    {
    public:
        enum Level
        {
            UNKNOWN = 0,
            DEBUG = 1,
            INFO = 2,
            WARNING = 3,
            ERROR = 4,
            FATAL = 5
        };

        // 给定由日志等级到文本输出的转换方法
        static const char* ToString(LogLevel::Level level);
    };

    // 日志格式器
    // 接收格式类似 %m %p %s %n等等
    class LogFormatter
    {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;

        LogFormatter(std::string pattern);
        // 对传送过来的event格式化后返回
        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    public:
        // 对接收到的格式进行解析
        class FormatItem
        {
        public:
            typedef std::shared_ptr<FormatItem> ptr;
            virtual ~FormatItem() {}
            virtual void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event);
        };

        void init();                            // 对m_pattern做解析，将解析出来的格式放入m_items中
    private:
        std::vector<FormatItem::ptr> m_items;   // 指定存储每一个格式 
        std::string m_pattern;                  // 指定格式器的格式, 构造函数中指定
        bool m_error;                           // 给定格式是否出错
    };

    // 日志输出地
    class LogAppender
    {
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        // 由于需要继承，所以设置析构为virtual
        virtual ~LogAppender() {}

        // 日志器中的log会来调用该层的log
        // 定义成为纯虚函数
        virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;

        void setFormatter(LogFormatter::ptr val) { m_formatter = val; }

        LogFormatter::ptr getFormatter() const { return m_formatter; }

    protected:
        LogLevel::Level m_level;
        LogFormatter::ptr m_formatter;
    };

    // 日志器
    class Logger
    {
    public:
        typedef std::shared_ptr<Logger> ptr;
        Logger(const std::string &name = "root");
        // log函数包含对应事件的等级，以及对应的事件
        void log(LogLevel::Level level, LogEvent::ptr event);

        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warning(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);

        LogLevel::Level getLevel() const { return m_level; }
        std::string getName() const { return m_name; }

        void setLevel(LogLevel::Level val) { m_level = val; }

    private:
        std::string m_name;                         // 日志器名称
        LogLevel::Level m_level;                    // 日志器默认设置的等级, 只有事件等级大于等于这个等级才会被打印
        std::list<LogAppender::ptr> m_logAppenders; // 日志输出地集合
    };

    // 输出到控制台的Appender
    class StdoutLogAppender : public LogAppender
    {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        void log(LogLevel::Level level, LogEvent::ptr event) override;

    private:
        
    };

    // 输出到文件的Appender
    class FileLogAppender : public LogAppender
    {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string &filename);
        void log(LogLevel::Level level, LogEvent::ptr event) override;

        // 重新打开文件
        bool reopen(); 

    private:
        std::string m_filename;
        std::ofstream m_filestream;
    };

}
#endif