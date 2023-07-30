#include "log.hpp"

namespace zy_log
{
    const char *LogLevel::ToString(LogLevel::Level level)
    {
        switch (level)
        {
// 通过定义宏函数简化代码
#define XX(name)         \
    case LogLevel::name: \
        return #name;    \
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
                i->log(level, event);   // logger中需要定义一个主日志器进行控制
            }
        }
    }

    FileLogAppender::FileLogAppender(const std::string &filename)
        : m_filename(filename) {}

    void StdoutLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level > m_level)
        {
            std::cout << m_formatter->format(logger, level, event);
        }
    }

    void FileLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level > m_level)
        {
            m_filestream << m_formatter->format(logger, level, event);
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
        : m_pattern(pattern) {}

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        std::stringstream res;
        for (auto &it : m_items)
        {
            it->format(res, logger, level, event);
        }
        return res.str();
    }

    /**
     * @brief 定义出LogFormatter的派生类
     * @param 
     */

    /*========================================================*/
    /*========================================================*/
    /*======================== start ===========================*/
    /*========================================================*/
    /*========================================================*/

    
    // 文件名
    class FileNameFormatItem : public LogFormatter::FormatItem
    {
    public:
        FileNameFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getFile();
        }
    };

    // 行号
    class LineFormatItem : public LogFormatter::FormatItem
    {
    public:
        LineFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getLine();
        }
    };

    // 程序开始启动的时间
    class ElapseFormatItem : public LogFormatter::FormatItem
    {
    public:
        ElapseFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getElapse();
        }
    };

    // 线程号
    class ThreadIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        ThreadIdFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getThreadId();
        }
    };

    // 协程号
    class FiberIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        FiberIdFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getFiberId();
        }
    };

    // 时间
    class TimeFormatItem : public LogFormatter::FormatItem
    {
    public:
        /**
         * @brief 时间类
         * 
         * @param 默认以年月日 时分秒格式输出时间, 以strtime提供的格式为准 
         */
        TimeFormatItem(const std::string& fmt = "%Y-%m-%d %H:%M:%S")
            :m_fmt(fmt) {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            struct tm tm;
            time_t time_stamp =  event->getTime(); // 将时间戳转化成为对应的格式输出
            localtime_r(&time_stamp, &tm);
            char buf[128];
            strftime(buf, sizeof(buf), m_fmt.c_str(), &tm);
            os << buf;
        }
    private:
        std::string m_fmt;
    };

    // 日志内容
    class ContentFormatItem : public LogFormatter::FormatItem
    {
    public:
        ContentFormatItem(const std::string& str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getContent();
        }
    };

    // 日志器名称
    class LoggerNameFormatItem : public LogFormatter::FormatItem
    {
    public:
        LoggerNameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << logger->getName();
        }
    };

    // 等级
    class LevelFormatItem : public LogFormatter::FormatItem
    {
    public:
        LevelFormatItem(const std::string& str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << level;
        }
    };

    class NewLineFormatItem : public LogFormatter::FormatItem
    {
    public:
        NewLineFormatItem(const std::string& str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << std::endl;
        }
    };

    class TabFormatItem : public LogFormatter::FormatItem
    {
    public:
        TabFormatItem(const std::string& str = "") {}
        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << "\t";
        }
    };


    /*========================================================*/
    /*========================================================*/
    /*======================== end ===========================*/
    /*========================================================*/
    /*========================================================*/

    void LogFormatter::init()
    {
        // 1. 先从m_pattern中提取格式
        // 格式可以是  %xxx 或者  %xxx{xxx}这两种形式，亦或是转义即%%, 其他的形式不允许
        // tuple中各元素的含义
        // 一号: 表示没有用花括号圈起来的格式
        // 二号: 表示用花括号圈起来了的格式
        // 三号: 为0时表示提取格式正确且
        std::vector<std::tuple<std::string, std::string, int>> fmt_vec;
        std::string all_str; // 存放所有有效数据

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

            // 检测到了% 并且不是转义字符
            size_t cur = i + 1;
            size_t f_status = 0;
            size_t f_begin = 0;

            std::string str; // 
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
        //  *  %m 消息
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

        static std::map<std::string, std::function<FormatItem::ptr(const std::string &str)>> s_format_items = {
#define XX(str, C) \
            {#str, [](const std::string &str) { return FormatItem::ptr(new C(#str)); }}

            XX(m, ContentFormatItem),    // %m 消息
            XX(p, LevelFormatItem),      // p:日志级别
            XX(r, ElapseFormatItem),     // r:累计毫秒数
            // XX(c, NameFormatItem),       // c:日志名称
            XX(t, ThreadIdFormatItem),   // t:线程id
            XX(n, NewLineFormatItem),    // n:换行
            XX(d, TimeFormatItem),   // d:时间
            XX(f, FileNameFormatItem),   // f:文件名
            XX(l, LineFormatItem),       // l:行号
            XX(T, TabFormatItem),        // T:Tab
            XX(F, FiberIdFormatItem),    // F:协程id
            // XX(N, ThreadNameFormatItem), // N:线程名称
#undef XX
        };
}