#include <iostream>

#include "log.h"

int main()
{
    LogEvent::ptr event(new LogEvent(
        LogLevel::INFO,		//日志级别
        __FILE__, 			//文件名称
        __LINE__, 		    //行号
        1234567, 			//程序运行时间
        2,					//线程ID
        3, 					//协程ID
        time(0)				//当前时间
    ));
    Logger::ptr lg(new Logger("zls"));
    lg->log(event);

    lg->setLevel(LogLevel::UNKNOW);

    // 添加输出地
    LogAppender::ptr stdout(new StdoutLogAppender());
    LogAppender::ptr file(new FileAppender("log.txt"));
    
    lg->addAppender(stdout);

    lg->log(event);

    return 0;
}