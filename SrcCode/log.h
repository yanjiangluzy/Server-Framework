#ifndef __ZY_LOG_H__
#define __ZY_LOG_H__


#include <iostream>
#include <cstring>
#include <memory>
#include <vector>
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>


using std::cout;
using std::endl;


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

    LogEvent(LogLevel::Level level, const char* file, int32_t m_line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time);

    // 一系列的set和get
    const char* getFile() const { return m_file;}
    int32_t getLine() const { return m_line;}
    uint32_t getElapse() const { return m_elapse;}
    uint32_t getThreadId() const { return m_threadId;}
    uint32_t getFiberId() const { return m_fiberId;}
    uint64_t getTime() const { return m_time;}
    LogLevel::Level getLevel() const { return m_level;}

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
};


// 日志输出器
class LogAppender
{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual void log(LogEvent::ptr event) = 0; 
    virtual ~LogAppender() {}
private:
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
    
    void addAppender(LogAppender::ptr appender);       // 添加输出地
    void delAppender(LogAppender::ptr appender);       // 删除输出地
private:
    std::string m_name;                                // 日志过滤器的名称

    LogLevel::Level m_level;                           // 日志过滤器的等级，低于该等级的日志不会被输出

    std::vector<LogAppender::ptr> m_appenders;         // 输出目的地
};



#endif
