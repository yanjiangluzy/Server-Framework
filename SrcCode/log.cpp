#include "log.hpp"

namespace zy_log
{
    const char * LogLevel::ToString(LogLevel::Level level)
    {
        // 通过定义宏函数简化代码
        switch(level)
        {
        #define XX(name) \
            case LogLevel::name: \
                return #name; \
                break; 
            
            XX(DEBUG);
            XX(INFO);
            XX(WARNING);
            XX(ERROR);
            XX(FATAL);
            
        // 取消宏函数定义
        #undef XX
            default:
                return "UNKNOWN";
        }
        return "UNKNOWN";
    }

    Logger::Logger(const std::string &name = "root")
        : m_name(name) {}

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
        :m_filename(filename) {}

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


    LogFormatter::LogFormatter(std::string pattern)
        :m_pattern(pattern) {}

    std::string LogFormatter::format(LogEvent::ptr event)
    {
        std::string res;
        for (auto &it : m_items)
        {
            res += it->format(event);
        }
        return res;
    }

    void LogFormatter::init()
    {
        // 1. 先从m_pattern中提取格式
        // 格式可以是  %xxx 或者  %xxx{xxx}这两种形式，亦或是转义即%%, 其他的形式不允许
        std::vector<std::tuple<std::string, std::string, int>> fmt_vec;
        std::string all_str;   // 存放所有有效数据

        for (size_t i = 0; i < m_pattern.size(); ++i)
        {
            if (m_pattern[i] != '%')
            {
                all_str.append(1, m_pattern[i]);
                continue;
            }

            if ((i + 1) < m_pattern.size() && m_pattern[i + 1] == '%')
            {
                all_str.append(1, '%');
                ++i;
                continue;
            }

            // 如果检测到了% 并且 不是转义字符
            size_t cur = i + 1;
            size_t f_status = 0;
            size_t f_begin = 0;

            std::string str; 
            std::string fmt; // 花括号中的内容{}

            while (cur < m_pattern.size())
            {
                if (!f_status && (!isalpha(m_pattern[cur]) && m_pattern[cur] != '{' && m_pattern[cur] != '}'))
                {
                    // cur是最后一个有效位置的下一个
                    str = m_pattern.substr(i + 1, cur - i - 1);
                    break;
                }

                // 正常字符
                if (!f_status && m_pattern[cur] == '{')
                {
                    fmt = m_pattern.substr(i + 1, cur - i - 1);
                    f_status = 1;
                    f_begin = cur + 1;
                    ++cur;
                    continue;
                }

                if (f_status && m_pattern[cur] == '}')
                {
                    str = m_pattern.substr(f_begin, cur - f_begin - 1);
                    f_status = 0;
                    ++cur;
                    break;
                }

                ++cur;
                if (cur == m_pattern.size())
                {
                    str = m_pattern.substr(i + 1, cur - i - 1);
                    break;
                }
            }
            // 到这里统一处理
            if (!f_status)
            {
                if (!all_str.empty())
                {
                    // 如果有冗余的字符串那么就要拼接上去
                    fmt_vec.push_back(std::make_tuple(all_str, std::string(), 0));
                    all_str.clear();
                }
                fmt_vec.push_back(std::make_tuple(str, fmt, 1));
            }
            else
            {
                // error
                fmt_vec.push_back(std::make_tuple("<error_pattern>", fmt, 0));
            }
        }

        // TODO
        if (!all_str.empty())
        {
            fmt_vec.push_back(std::make_tuple(all_str, std::string(), 0));
        }

        // 2. 根据提取出来的格式进行转换
        // *  %m 消息
        //  *  %p 日志级别
        //  *  %r 累计毫秒数
        //  *  %c 日志名称
        //  *  %t 线程id
        //  *  %n 换行
        //  *  %d 时间
        //  *  %f 文件名
        //  *  %l 行号
        //  *  %T 制表符
        //  *  %F 协程id
        //  *  %N 线程名称
    }


}