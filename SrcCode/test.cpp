#include <iostream>

#include "log.h"

int main()
{
    // LogEvent::ptr event(new LogEvent(
    //     LogLevel::INFO,		//日志级别
    //     __FILE__, 			//文件名称
    //     __LINE__, 		    //行号
    //     1234567, 			//程序运行时间
    //     2,					//线程ID
    //     3, 					//协程ID
    //     time(0),				//当前时间
    //     "root"
    // ));
    // Logger::ptr lg(new Logger("zls"));

    // LogEventWrap(lg, event);
    // lg->log(event);

    // lg->setLevel(LogLevel::UNKNOW);

    // // 添加输出地
    // LogAppender::ptr stdout(new StdoutLogAppender());
    // LogAppender::ptr file(new FileAppender("log.txt"));

    // event->getSs() << "heihei";
    
    // lg->addAppender(stdout);

    // lg->log(event);

    // LogFormatter::ptr formatter(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));

    // cout << formatter->format(event) << endl;


    // Logger::ptr lg(new Logger("zls"));

    // LOG_LEVEL_CPP(lg, LogLevel::DEBUG) << "hello mylog";

    // LOG_LEVEL_C(lg, LogLevel::INFO, "hello mylog");

    
    LOG_LEVEL_CPP(Manager::getSingletion()->getRoot(), LogLevel::INFO) << "last test";


    return 0;
}