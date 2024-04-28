#ifndef __ZY_LOG_H__
#define __ZY_LOG_H__


#include <iostream>
#include <cstring>
#include <string>
#include <memory>
#include <vector>
#include <tuple>
#include <ctime>
#include <sstream>
#include <dbg.h>
#include <map>
#include <functional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include "singleton.h"


using std::cout;
using std::endl;

// 通过宏定义简化调用
// c++风格的宏定义
#define LOG_LEVEL_CPP(logger, level) \
    LogEventWrap(logger, LogEvent::ptr(new LogEvent(                  \
                 level, __FILE__, __LINE__, 0,    \
                syscall(SYS_gettid), 1, time(0), logger->getName())))                  \
                .getSs()

// C语言风格的宏定义
#define LOG_LEVEL_C(logger, level, message) \
    LogEventWrap(logger, LogEvent::ptr(new LogEvent(level, __FILE__, __LINE__, 0,    \
                syscall(SYS_gettid), 1, time(0), logger->getName())))                  \
                .getSs() << message



// 包含日志等级
class LogLevel
{
public:
    enum Level
    {   
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,   
        WARNING = 3,
        ERROR = 4,
        FATAL = 5
    };

    static const std::string toString(LogLevel::Level level);

private:
};

// 日志事件
// 控制要输出的日志内容
// 时间 线程号 协程号 日志等级 文件 行号 主体内容 
class LogEvent
{
public:
    typedef std::shared_ptr<LogEvent> ptr;

    LogEvent(LogLevel::Level level, const char* file, int32_t m_line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time, std::string logger_name);

    // 一系列的set和get
    const char* getFile() const { return m_file;}
    int32_t getLine() const { return m_line;}
    uint32_t getElapse() const { return m_elapse;}
    uint32_t getThreadId() const { return m_threadId;}
    uint32_t getFiberId() const { return m_fiberId;}
    uint64_t getTime() const { return m_time;}
    LogLevel::Level getLevel() const { return m_level;}
    std::string getLoggerName() const { return m_logger_name; }
    std::string getContext() const { return m_message.str(); }
    std::stringstream& getSs() { return m_message; }


    // 提供时间戳转化成年月日
    static const std::string getLocalTimeFromTimestamp(time_t timestamp); 


private:
    LogLevel::Level m_level;       //日志级别           
    const char* m_file = nullptr;  //文件名
    int32_t m_line = 0;            //行号
    uint32_t m_elapse = 0;         //程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;       //线程id
    uint32_t m_fiberId = 0;        //协程id
    uint64_t m_time = 0;           //时间戳
    std::string m_logger_name;     //日志器名称
    std::stringstream m_message;   //定制消息 
};



// 日志格式器
// 让用户能够自定义格式
// 格式："%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
// *  %m 消息
// *  %p 日志级别
// *  %r 累计毫秒数
// *  %c 日志名称
// *  %t 线程id
// *  %n 换行
// *  %d 时间
// *  %f 文件名
// *  %l 行号
// *  %T 制表符
// *  %F 协程id

class LogFormatter
{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& m_pattern);

    void init();                                                     // 初始化，解析传入的格式

    std::string format(LogEvent::ptr event);


    // 定义类，用于解析各种格式对应的内容
    class FormatItem
    {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        FormatItem() {}
        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, LogEvent::ptr ptr) = 0; 
    private:
    };

    ~LogFormatter() {}
private:
    const std::string m_pattern;
    std::vector<std::tuple<std::string, std::string, int>> m_items;  // 存储形式 普通字符 其子串 是否存在     
    std::vector<FormatItem::ptr> m_formats;                    
};

// 保证接口的统一
class MessageFormatItem : public LogFormatter::FormatItem
{
public:
    MessageFormatItem(const std::string& str = "") {}
    virtual void format(std::ostream& os, LogEvent::ptr event) override
    {
        os << event->getContext();
    }
private:
};

class LevelFormatItem : public LogFormatter::FormatItem {
public:
    LevelFormatItem(const std::string& str = "") {}
    virtual void format(std::ostream& os, LogEvent::ptr event) override 
    {
        os << LogLevel::toString(event->getLevel());
    }
};


class ElapseFormatItem : public LogFormatter::FormatItem {
public:
    ElapseFormatItem(const std::string& str = "") {}
    virtual void format(std::ostream& os, LogEvent::ptr event) override
    {
        os << event->getElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem {
public:
    NameFormatItem(const std::string& str = "") {}
    virtual void format(std::ostream& os, LogEvent::ptr event) override 
    {
        os << event->getLoggerName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem {
public:
    ThreadIdFormatItem(const std::string& str = "") {}
    virtual void format(std::ostream& os, LogEvent::ptr event) override 
    {
        os << event->getThreadId();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem {
public:
    FiberIdFormatItem(const std::string& str = "") {}
    virtual void format(std::ostream& os, LogEvent::ptr event) override 
    {
        os << event->getFiberId();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
        :m_format(format) {
        if(m_format.empty()) {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }

    virtual void format(std::ostream& os, LogEvent::ptr event) override 
    {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    std::string m_format;
};

class FilenameFormatItem : public LogFormatter::FormatItem {
public:
    FilenameFormatItem(const std::string& str = "") {}
    virtual void format(std::ostream& os, LogEvent::ptr event) override 
    {
        os << event->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem {
public:
    LineFormatItem(const std::string& str = "") {}
    virtual void format(std::ostream& os, LogEvent::ptr event) override 
    {
        os << event->getLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
public:
    NewLineFormatItem(const std::string& str = "") {}
    virtual void format(std::ostream& os, LogEvent::ptr event) override 
    {
        os << std::endl;
    }
};

class StringFormatItem : public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string& str)
        :m_string(str) {}
    virtual void format(std::ostream& os, LogEvent::ptr event) override 
    {
        os << m_string;
    }
private:
    std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem {
public:
    TabFormatItem(const std::string& str = "") {}
    virtual void format(std::ostream& os, LogEvent::ptr event) override 
    {
        os << "\t";
    }
private:
    std::string m_string;
};


// 日志输出器
class LogAppender
{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual void log(LogEvent::ptr event) = 0; 
    virtual ~LogAppender() {}
    void setFormatter(LogFormatter::ptr val) { m_formatter = val;}
    LogFormatter::ptr& getFormatter() { return m_formatter; }
private:
    LogFormatter::ptr m_formatter;
};

// 输出到控制台
class StdoutLogAppender : public LogAppender
{
public:
    virtual void log(LogEvent::ptr event) override;
    virtual ~StdoutLogAppender() {}
private:
};

// 输出到文件
class FileAppender : public LogAppender
{
public:
    FileAppender(const std::string file);
    virtual void log(LogEvent::ptr event) override;
    virtual ~FileAppender() {}
private:
    std::string m_file;                                // 目的文件名称
};

// 日志器
// 1. 对日志进行过滤
// 2. 对符合条件的日志进行输出
class Logger
{
public:
    typedef std::shared_ptr<Logger> ptr;
    Logger(const std::string name = "root", LogLevel::Level level = LogLevel::Level::DEBUG);
    void log(LogEvent::ptr event);                     // 参数是代表当前想要输出的日志等级，如果低于当前日志器的level则不会输出  
    
    void setLevel(LogLevel::Level level);              // 重新设置过滤等级  

    void setFormat(LogFormatter::ptr format);
    
    void addAppender(LogAppender::ptr appender);       // 添加输出地
    void delAppender(LogAppender::ptr appender);       // 删除输出地

    const std::string& getName() { return m_name; }    
private:
    std::string m_name;                                // 日志过滤器的名称

    LogLevel::Level m_level;                           // 日志过滤器的等级，低于该等级的日志不会被输出

    std::vector<LogAppender::ptr> m_appenders;         // 输出目的地
};


class LogEventWrap
{
public:
    LogEventWrap(Logger::ptr logger, LogEvent::ptr event);
    std::stringstream& getSs();
    ~LogEventWrap(); 
private:
    Logger::ptr m_logger; 
    LogEvent::ptr m_event;
};


// 日志管理
class LogManager
{
public:
    LogManager();
    
    Logger::ptr getLogger(const std::string& name);

    Logger::ptr getRoot();

private:
    Logger::ptr m_root;
    std::vector<Logger::ptr> m_loggers; 
};

typedef SingletionPtr<LogManager> Manager;
 

#endif
