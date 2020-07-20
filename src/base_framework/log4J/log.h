#ifndef __LOG4J__
#define __LOG4J__

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <vector>
#include <fstream>

namespace log {

// 日志事件
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();
private:
    const char* m_filename = nullptr;  // 文件名
    uint32_t m_line = 0;  // 行号
    uint32_t m_elapse = 0;  // 程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;  // 线程id
    uint32_t m_fiberId = 0;  // 协程id
    uint64_t m_time; // 时间戳
    std::string m_content;  
};

// 日志级别
class LogLevel {
public:
    enum Level {
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
};

// 日志输出格式
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);
    // 
    std::string Format(LogEvent::ptr event);
    // 日志格式解析
    void Init();
public:
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem() {}
        virtual void Format(std::stringstream os, LogEvent::ptr event) = 0;
    };
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items; 
};

// 日志输出地
class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {}
    virtual void Log(LogLevel::Level level, LogEvent::ptr event) = 0;
    void SetFormatter(LogFormatter::ptr formatter) { m_formatter = formatter; }
    LogFormatter::ptr GetFormatter() const { return m_formatter; }
protected:
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
};

// 日志输出器
class Logger {
public:
    typedef std::shared_ptr<Logger> ptr;
    Logger(const std::string& name = "root");
    void Log(LogLevel::Level level, const LogEvent::ptr event);
    void Debug(LogEvent::ptr event);
    void Info(LogEvent::ptr event);
    void Warn(LogEvent::ptr event);
    void Error(LogEvent::ptr event);
    void Fatal(LogEvent::ptr event);
    void AddAppender(LogAppender::ptr appender);
    void DelAppender(LogAppender::ptr appender);
    LogLevel::Level GetLevel() const { return m_level; }
    void SetLevel(LogLevel::Level level) { m_level = level; }
private:
    std::string m_name;  // 日志名称
    LogLevel::Level m_level;  // 日志级别
    std::list<LogAppender::ptr> m_appenders; // Appender集合
};

// 输出到控制台的Appender
class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void Log(LogLevel::Level level, LogEvent::ptr event) override;
private:

};

// 输出到文件的Appender
class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void Log(LogLevel::Level level, LogEvent::ptr event) override;
    // 文件打开成功返回true
    bool Reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};

}

#endif

