
#include "log.h"

const std::string LogLevel::toString(LogLevel::Level level)
{
    switch (level)
    {
#define XX(name) \
    case LogLevel::name: \
        return #name;\
        break;

    XX(UNKNOW);
    XX(DEBUG);
    XX(INFO);
    XX(WARNING);
    XX(ERROR);
    XX(FATAL);

#undef XX
    
    default:
        return "UNKOWN REASON"; 
        break;
    }
    return "UNKOWN REASON";
}

LogEvent::LogEvent(LogLevel::Level level, const char* file, int32_t line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time)
    : m_level(level), m_file(file), m_line(line), m_elapse(elapse), m_threadId(thread_id), m_fiberId(fiber_id), m_time(time)
{}

const std::string LogEvent::getLocalTimeFromTimestamp(time_t timestamp)
{
	//格式化时间
    const std::string format = "%Y-%m-%d %H:%M:%S";
    struct tm tm;
    time_t t = timestamp;
    localtime_r(&t, &tm);
    char tm_buf[64];
    strftime(tm_buf, sizeof(tm_buf), format.c_str(), &tm); 

    return tm_buf;
}

Logger::Logger(const std::string name, LogLevel::Level level)
    : m_name(name), m_level(level)
{}

void Logger::log(LogEvent::ptr event)
{
    if (event->getLevel() >= m_level)
    {
        for (int i = 0; i < m_appenders.size(); ++i)
        {
            m_appenders[i]->log(event);
        }
    }
    else 
        cout << "不满足 " << endl;
}

void Logger::setLevel(LogLevel::Level level)
{
    m_level = level;
}

void Logger::addAppender(LogAppender::ptr appender)
{
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender)
{
    for (int i = 0; i < m_appenders.size(); ++i)
    {
        if (m_appenders[i] == appender) 
        {
            m_appenders.erase(m_appenders.begin() + i);
            break;
        }
    }
}

void StdoutLogAppender::log(LogEvent::ptr event)
{
      std::cout
        << LogEvent::getLocalTimeFromTimestamp(event->getTime()) << "  "
        << getpid() << "  "
        << event->getFiberId() << "  "
        << "["
        << LogLevel::toString(event->getLevel())
        << "] "
        << event->getFile() << ":" << event->getLine() << "  "
        << "输出到控制台的信息"
        << std::endl;
}

void FileAppender::log(LogEvent::ptr event) 
{
    cout << "输出到文件" << endl;
}

FileAppender::FileAppender(const std::string file)
    : m_file(file)
{}


