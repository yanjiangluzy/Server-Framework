#include <iostream>
#include <cstring>


using std::cout;
using std::endl;


// 包含日志等级
class LogLevel
{
public:
    enum Level
    {   
        UNKNOW = 0,
        NORMAL = 1
        DEBUG = 2,
        WARNING = 3,
        ERROR = 4,
        FATAL = 5
    };
private:
};

// 日志过滤器
class LogFilter
{
public:
    LogFilter()
private:
};