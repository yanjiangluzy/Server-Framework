
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

LogEvent::LogEvent(LogLevel::Level level, const char* file, int32_t line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time, std::string logger_name)
    : m_level(level), m_file(file), m_line(line), m_elapse(elapse), m_threadId(thread_id), m_fiberId(fiber_id), m_time(time), m_logger_name(logger_name)
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


LogFormatter::LogFormatter(const std::string& pattern)
    : m_pattern(pattern)
{
    init();
}


void LogFormatter::init()
{
    // 对m_pattern做解析, 解析出各个参数
    // TODO
    std::string nstr;                               // 无关字符存储于此
    for (size_t i = 0; i < m_pattern.size(); ++i)
    {
        if (m_pattern[i] != '%')
        {
            nstr.append(1, m_pattern[i]);
            continue;
        }
        
        // 如果是"%", 那么有可能下一个还是%，那么把这个%放到无关字符中
        if ((i + 1) < m_pattern.size() && m_pattern[i + 1] == '%')
        {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        // 如果%后边不是% 那么需要进行解析，后边可能是大括号，也有可能是字母
        std::string fmt;               // 存储其子串
        std::string str;               // 存储字母
        size_t n = i + 1;              // 从%的后边一个字母开始解析
        int fmt_status = 0;            // 代表是否解析到大括号
        size_t fmt_begin = -1;         // 代表大括号开始的位置

        while (n < m_pattern.size())
        {
            // 如果后边既不是字母，又不是大括号，且此时没有解析到大括号内那么可能是%
            if (!fmt_status && (!isalpha(m_pattern[n])) && m_pattern[n] != '{' && m_pattern[n] != '}')
            {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }

            // 如果还没解析到大括号, 那么就去检查后边是不是大括号
            if (!fmt_status)
            {
                if (m_pattern[n] == '{')
                {
                    // 说明解析到了大括号
                    if (str.empty()) 
                    {
                        str = m_pattern.substr(i + 1, n - i - 1);
                    }
                    fmt_status = 1;
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            }
            else
            {
                // 当前正在解析大括号内的内容
                // 检查是否结束
                if (m_pattern[n] == '}')
                {
                    // {1234}
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    fmt_status = 0;    // 代表解析完毕
                    ++n;                
                    break;
                }
            }
            ++n;

            if (n == m_pattern.size())
            {
                // 说明此时解析到了结尾，那么将解析到的字符串全部塞到str中去
                // "123"
                if (str.empty())
                {
                    str = m_pattern.substr(i + 1, n - i - 1);
                }
            }
        }// while done

        if (fmt_status == 0)
        { 
            // 可能解析到了大括号 可能没解析到，但是无所谓，都可以用下列代码插入
            // 说明解析到了大括号
            // 在此之前的多余字符还没有被存储
            if (!nstr.empty())
            {
                m_items.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            m_items.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        }
        else if (fmt_status == 1)
        {
            // 说明解析错误
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            m_items.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }// for done


    // dbg(m_pattern);
    if (!nstr.empty())
    {
        m_items.push_back(std::make_tuple(nstr, "", 0));
    }

    // for(auto& it : m_items) {
        
    //     std::cout 
    //     << std::get<0>(it) 
    //     << " : " << std::get<1>(it) 
    //     << " : " << std::get<2>(it)
    //     << std::endl;
    // }

    // 解析完毕后，将对应的解析器构造函数与对应字符绑定
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>> items = {
#define XX(str, C) \
    {#str, [](const std::string& fmt){ return FormatItem::ptr(new C(fmt)); }}

    XX(m, MessageFormatItem),
    XX(p, LevelFormatItem),
    XX(r, ElapseFormatItem),
    XX(c, NameFormatItem),
    XX(t, ThreadIdFormatItem),
    XX(n, NewLineFormatItem),
    XX(d, DateTimeFormatItem),
    XX(f, FilenameFormatItem),
    XX(l, LineFormatItem),
    XX(T, TabFormatItem),
    XX(F, FiberIdFormatItem)

#undef XX
    };

    // 输出看看
    for (auto& it : m_items)
    {
        if (std::get<2>(it) == 0)
        {
            // 说明是无用字符, 放到stringitem中
            m_formats.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(it))));
        }
        else
        {
            // 说明是格式字符，找到对应解析器
            auto format_item = items.find(std::get<0>(it));
            if (format_item == items.end())
            {
                // 找不到对应的解析器
                m_formats.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(it) + ">>")));
            }
            else
            {
                m_formats.push_back(format_item->second(std::get<1>(it)));
            }
        }
    }
}


std::string LogFormatter::format(LogEvent::ptr event)
{
    // 通过stringstream来完成字符串之间的转换叠加
    std::stringstream ss;
    for(auto& i : m_formats) 
    {
        i->format(ss,event);
    }
    return ss.str();
}


Logger::Logger(const std::string name, LogLevel::Level level)
    : m_name(name), m_level(level)
{}

void Logger::log(LogEvent::ptr event)
{
    // 如果当前日志器没有添加输出地，那么自动添加控制台输出
    if (m_appenders.size() == 0)
    {
        m_appenders.push_back(LogAppender::ptr(new StdoutLogAppender()));
    }

    if (event->getLevel() >= m_level)
    {
        for (int i = 0; i < m_appenders.size(); ++i)
        {
            m_appenders[i]->log(event);
        }
    }
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

void Logger::setFormat(LogFormatter::ptr format)
{
    for (auto& it : m_appenders)
    {
        it->setFormatter(format);
    }
}

void StdoutLogAppender::log(LogEvent::ptr event)
{
    if (!getFormatter().get())
    {
        // 没有设置格式, 那就设置一个默认格式
        setFormatter(LogFormatter::ptr(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n")));
    }

    cout << getFormatter()->format(event) << endl;
}

void FileAppender::log(LogEvent::ptr event) 
{
    cout << "输出到文件" << endl;
}

FileAppender::FileAppender(const std::string file)
    : m_file(file)
{}



LogEventWrap::LogEventWrap(Logger::ptr logger, LogEvent::ptr event)
    : m_logger(logger), m_event(event)
{}

LogEventWrap::~LogEventWrap()
{
    // 析构时自动调用log
    m_logger->log(m_event);
}


std::stringstream& LogEventWrap::getSs()
{
    return m_event->getSs();
}


LogManager::LogManager()
{
    m_root = Logger::ptr(new Logger("root"));
}


Logger::ptr LogManager::getLogger(const std::string& name)
{
    for (auto& it : m_loggers)
    {
        if (it->getName() == name)
        {
            return it;
        }
    }
    return nullptr;
}

Logger::ptr LogManager::getRoot()
{
    return m_root;
}


