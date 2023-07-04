#include <iostream>
#include <memory>
// 测试

class LogEvent
{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();

private:
    const char* m_file = nullptr;
    int32_t m_line = 0;
    uint32_t m_elapse = 0;
    uint32_t m_threadId = 0;
    uint32_t m_fiberId = 0;
    uint64_t m_time = 0;
    std::string m_content;     // string一般和指针的大小一样，都是4 or 8字节
}; 