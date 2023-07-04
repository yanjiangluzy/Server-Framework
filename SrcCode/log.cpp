#include "log.hpp"

namespace zy_log
{
    Logger::Logger(const std::string &name = "root")
        : m_name(name)
    {
        ;
    }

    void Logger::addAppender(LogAppender::ptr appender)
    {
        m_logAppenders.push_back(appender);
    }

    void Logger::delAppender(LogAppender::ptr appender)
    {
        for (auto it = m_logAppenders.begin(); it != m_logAppenders.end(); ++it)
        {
            if ((*it) == appender)
            {
                m_logAppenders.erase(it);
                break;
            }
        }
    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event)
    {
        // 只有当需要打印的日志事件等级大于等于当前日志器设置的等级时才会打印
        if (level >= m_level)
        {
            for (auto &i : m_logAppenders)
            {
                i->log(level, event);
            }
        }
    }

    FileLogAppender::FileLogAppender(const std::string& filename)
        :m_filename(filename)
    {
    }

    void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level > m_level)
        {
            std::cout << m_formatter->format(event);
        }
    }

    
    void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level > m_level)
        {
            m_filestream << m_formatter->format(event);
        }
    }

    bool FileLogAppender::reopen()
    {
        if (m_filestream)
        {
            m_filestream.close();
        }
        m_filestream.open(m_filename);

        // 这里的双感叹号表示，0值还是0，但是非0值全部变成1
        return !!m_filestream; 
    }
}