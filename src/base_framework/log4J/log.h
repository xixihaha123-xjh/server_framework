#ifndef __LOG4J__
#define __LOG4J__

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <stdarg.h>

#include "../../../src/base_framework/utility/singleton.h"
#include "../../../src/base_framework/utility/utility.h"

#define LOG_LEVEL(logger, level) \
    if(logger->GetLevel() <= level) \
        logging::LogEventWarp(logging::LogEvent::ptr(new logging::LogEvent(logger, level, __FILE__, __LINE__, 1, \
                        utility::GetThreadId(), utility::GetFiberId(), time(0)))).GetLogEventStream()

#define LOG_DEBUG(logger) LOG_LEVEL(logger, logging::LogLevel::DEBUG)
#define LOG_INFO(logger) LOG_LEVEL(logger, logging::LogLevel::INFO)
#define LOG_WARN(logger) LOG_LEVEL(logger, logging::LogLevel::WARN)
#define LOG_ERROR(logger) LOG_LEVEL(logger, logging::LogLevel::ERROR)
#define LOG_FATAL(logger) LOG_LEVEL(logger, logging::LogLevel::FATAL)

#define LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->GetLevel() <= level) \
        logging::LogEventWarp(logging::LogEvent::ptr(new logging::LogEvent(logger, level, __FILE__, __LINE__, 1, \
                        utility::GetThreadId(), utility::GetFiberId(), time(0)))).GetLogEvent()->format(fmt, __VA_ARGS__)

#define LOG_FMT_DEBUG(logger, fmt, ...) LOG_FMT_LEVEL(logger, logging::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define LOG_FMT_INFO(logger, fmt, ...) LOG_FMT_LEVEL(logger, logging::LogLevel::INFO, fmt, __VA_ARGS__)
#define LOG_FMT_WARN(logger, fmt, ...) LOG_FMT_LEVEL(logger, logging::LogLevel::WARN, fmt, __VA_ARGS__)
#define LOG_FMT_ERROR(logger, fmt, ...) LOG_FMT_LEVEL(logger, logging::LogLevel::ERROR, fmt, __VA_ARGS__)
#define LOG_FMT_FATAL(logger, fmt, ...) LOG_FMT_LEVEL(logger, logging::LogLevel::FATAL, fmt, __VA_ARGS__)

#define LOG_ROOT() logging::loggerMgr::GetInstance()->GetRoot()

namespace logging {

class Logger;

// 日志级别
class LogLevel {
public:
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
    static const char* ToString(LogLevel::Level level);
};

// 日志事件
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* fileName, int32_t line, uint32_t elapse, 
             uint32_t threadId, uint32_t fiberId, uint64_t time);

    const char* GetFileName() const { return m_filename; }
    uint32_t GetLine() const { return m_line; }
    uint32_t GetElapse() const { return m_elapse; }
    uint32_t GetThreadId() const { return m_threadId; }
    uint32_t GetFiberId() const { return m_fiberId; }
    uint64_t GetTime() const { return m_time; }
    // 返回日志内容
    std::string GetContent() const { return m_content.str(); }
    // 返回日志内容字符串流
    std::stringstream& GetContentStream() { return m_content; }
    std::shared_ptr<Logger> GetLogger() const { return m_logger; }
    LogLevel::Level GetLogLevel() const { return m_level; }
    void format(const char* fmt, va_list al); // va_list 获取可变参数列表的第一个参数的地址
    void format(const char* fmt, ...);
private:
    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
    const char* m_filename = nullptr;  // 文件名
    uint32_t m_line = 0;  // 行号
    uint32_t m_elapse = 0;  // 程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;  // 线程id
    uint32_t m_fiberId = 0;  // 协程id
    uint64_t m_time; // 时间戳
    std::stringstream m_content; // 日志内容流
};

class LogEventWarp {
public:
    LogEventWarp(LogEvent::ptr logevent);
    ~LogEventWarp();
    LogEvent::ptr GetLogEvent() const { return m_logevent; }
    std::stringstream& GetLogEventStream();
private:
    LogEvent::ptr m_logevent;
};

// 日志输出格式
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);
    // 
    std::string Format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    // 日志格式解析
    void Init();
    // 解析pattern (%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n)
private:
    void ParsePattern(std::vector<std::tuple<std::string, std::string, int>> &vec);
    // 解析格式项
    void ParseFormatItem(std::vector<std::tuple<std::string, std::string, int>> vec);
public:
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem() {}
        virtual void Format(std::stringstream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
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
    virtual void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    void SetFormatter(LogFormatter::ptr formatter) { m_formatter = formatter; }
    LogFormatter::ptr GetFormatter() const { return m_formatter; }
    LogLevel::Level GetLevel() const { return m_level; }
    void SetLevel(LogLevel::Level level) { m_level = level; }
protected:
    LogLevel::Level m_level = LogLevel::DEBUG;
    LogFormatter::ptr m_formatter;
};

// 日志输出器
class Logger : public std::enable_shared_from_this<Logger> {
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
    const std::string& GetName() const { return m_name; }

private:
    std::string m_name;  // 日志名称
    LogLevel::Level m_level;  // 日志级别
    std::list<LogAppender::ptr> m_appenders; // Appender集合
    LogFormatter::ptr m_formatter;
};

// 输出到文件的Appender
class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    // 文件打开成功返回true
    bool Reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};

// 输出到控制台的Appender
class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
private:

};

class LoggerManager
{
public:
    LoggerManager();
    Logger::ptr GetLogger(const std::string& name);
    void init();
    Logger::ptr GetRoot() const { return m_root; }
private:
    std::map<std::string, Logger::ptr> m_loggermap;
    Logger::ptr m_root;
};

typedef single::Singleton<LoggerManager> loggerMgr;

}

#endif

