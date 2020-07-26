// 建议按照稳定度排序：
// cpp 对应的头文件, C/C++标准库, 系统库的.h, 其他库的.h, 本项目内其他的.h
#include "log.h"

#include <map>
#include <sstream>
#include <iostream>
#include <functional>
#include <time.h>
#include <string.h>
#include <stdio.h>

namespace logging {

const char* LogLevel::ToString(LogLevel::Level level)
{
    switch(level) {
#define XX(name) \
        case LogLevel::name: \
            return #name; \
            break;
        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
#undef XX
    default:
        return "UNKNOW";
    }
    return "UNKNOW";
}

class MessageFormatItem : public LogFormatter::FormatItem 
{
public:
    MessageFormatItem(const std::string& fmt = "") {};
    void Format(std::stringstream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->GetContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem 
{
public:
    LevelFormatItem(const std::string& fmt = "") {};
    void Format(std::stringstream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem 
{
public:
    ElapseFormatItem(const std::string& fmt = "") {};
    void Format(std::stringstream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->GetElapse();
    }
};

class LogNameFormatItem : public LogFormatter::FormatItem 
{
public:
    LogNameFormatItem(const std::string& fmt = "") {};
    void Format(std::stringstream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << logger->GetName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem 
{
public:
    ThreadIdFormatItem(const std::string& fmt = "") {};
    void Format(std::stringstream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->GetThreadId();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem 
{
public:
    FiberIdFormatItem(const std::string& fmt = "") {};
    void Format(std::stringstream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->GetFiberId();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem 
{
public:
    DateTimeFormatItem(const std::string& timeformat = "%Y-%m-%d %H:%M:%S") : m_timeformat(timeformat) {
        if (m_timeformat.empty()) {
            m_timeformat = "%Y-%m-%d %H:%M:%S";
        }
    }
    void Format(std::stringstream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        struct tm now_time;
        time_t time = event->GetTime();
        localtime_r(&time, &now_time);
        char buf[80];
        strftime(buf, sizeof(buf), m_timeformat.c_str(), &now_time);
        os << buf;
    }
private:
    std::string m_timeformat;
};

class FileNameFormatItem : public LogFormatter::FormatItem 
{
public:
    FileNameFormatItem(const std::string& fmt = "") {};
    void Format(std::stringstream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->GetFileName();
    }
};

class LineFormatItem : public LogFormatter::FormatItem 
{
public:
    LineFormatItem(const std::string& fmt = "") {};
    void Format(std::stringstream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << event->GetLine();
    }
};

class StringFormatItem : public LogFormatter::FormatItem 
{
public:
    StringFormatItem(const std::string& str) : m_string(str) {};
    void Format(std::stringstream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << m_string;
    }
private:
    std::string m_string;
};

class TableFormatItem : public LogFormatter::FormatItem 
{
public:
    TableFormatItem(const std::string& fmt = "") {};
    void Format(std::stringstream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << "  ";
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem 
{
public:
    NewLineFormatItem(const std::string& fmt = "") {};
    void Format(std::stringstream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << std::endl;
    }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem 
{
public:
    ThreadNameFormatItem(const std::string& fmt = "") {};
    void Format(std::stringstream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
    {
        os << "ThradName";
    }
};

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* fileName, int32_t line, uint32_t elapse,
 uint32_t threadId, uint32_t fiberId, uint64_t time)
    : m_logger(logger)
    , m_level(level)
    , m_filename(fileName)
    , m_line(line)
    , m_elapse(elapse)
    , m_threadId(threadId)
    , m_fiberId(fiberId)
    , m_time(time)
{
}

void LogEvent::format(const char* fmt, va_list al)
{
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if (len != -1) {
        m_content << std::string(buf ,len);
        free(buf);
    }
}

void LogEvent::format(const char* fmt, ...)
{
    va_list al;
    va_start(al, fmt);
    format (fmt, al);
    va_end(al);
}

LogEventWarp::LogEventWarp(LogEvent::ptr logevent) : m_logevent(logevent)
{
}

LogEventWarp::~LogEventWarp()
{
    m_logevent->GetLogger()->Log(m_logevent->GetLogLevel(), m_logevent);
}

std::stringstream& LogEventWarp::GetLogEventStream()
{
    return m_logevent->GetContentStream();
}

Logger::Logger(const std::string& name) 
    : m_name(name)
    , m_level(LogLevel::DEBUG)
{
    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}

void Logger::AddAppender(LogAppender::ptr appender)
{
    if (!appender->GetFormatter()) {
        appender->SetFormatter(m_formatter);
    }
    m_appenders.push_back(appender);
}

void Logger::DelAppender(LogAppender::ptr appender)
{
    for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it) {
        if (*it == appender) {
            m_appenders.erase(it);
        }
    }
}

void Logger::Log(LogLevel::Level level, const LogEvent::ptr event)
{
    auto self = shared_from_this();
    if (level >= m_level) {
        for (auto &i : m_appenders) {
            i->Log(self, level, event);
        }
    }
}

void Logger::Debug(LogEvent::ptr event)
{
    Log(LogLevel::DEBUG, event);
}

void Logger::Info(LogEvent::ptr event)
{
    Log(LogLevel::INFO, event);
}

void Logger::Warn(LogEvent::ptr event)
{
    Log(LogLevel::WARN, event);
}

void Logger::Error(LogEvent::ptr event)
{
    Log(LogLevel::ERROR, event);
}

void Logger::Fatal(LogEvent::ptr event)
{
    Log(LogLevel::FATAL, event);
}

void StdoutLogAppender::Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= m_level) {
        std::cout << m_formatter->Format(logger, level, event);
    }

}

FileLogAppender::FileLogAppender(const std::string& filename) : m_filename(filename)
{
    Reopen();
}

bool FileLogAppender::Reopen()
{
    if (m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return true;
}

void FileLogAppender::Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= m_level) {
        m_filestream << m_formatter->Format(logger, level, event);
    }
}

LogFormatter::LogFormatter(const std::string& pattern) : m_pattern(pattern)
{
    // std::cout << __FILE__ << "  "<< __LINE__ << "  " << "LogFormatter::LogFormatter" << std::endl;
    Init();
}

std::string LogFormatter::Format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    std::stringstream ss;
    for (auto& i : m_items) {
        i->Format(ss, logger, level, event);
    }
    // std::cout << "-------"<< ss.str() << "---------" << std::endl;
    return ss.str();
}

void LogFormatter::Init()
{
    // str  format  type
    // m_pattern: %d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n
    // str 正常格式:d T t F l ...  异常格式: %
    // format: 存放FormatItem
    // type 0表示解析正常，1表示解析异常
    std::vector<std::tuple<std::string, std::string, int>> vec;
    ParsePattern(vec);
    
    ParseFormatItem(vec);
}

void LogFormatter::ParsePattern(std::vector<std::tuple<std::string, std::string, int> > &vec)
{
    // str  format  type
    // m_pattern: %d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n
    // format 
    // std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string abnormalStr;
    for (size_t i = 0; i < m_pattern.size(); ++i) {
        if (m_pattern[i] != '%') {
            abnormalStr.append(1, m_pattern[i]);
            continue;
        }

        if ((i + 1) < m_pattern.size()) {
            if (m_pattern[i + 1] == '%') {
                abnormalStr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmtStatus = 0;   // 解析状态,为0解析正常,为1解析不正常
        size_t fmtBegin = 0; // 取出时间str

        std::string formatStr;
        std::string fmt;
        while (n < m_pattern.size()) {
            if (!fmtStatus && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '{')) {
                formatStr = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if (fmtStatus == 0) {
                if (m_pattern[n] == '{') {
                    formatStr = m_pattern.substr(i + 1, n - i - 1);
                    fmtStatus = 1;
                    ++n;
                    fmtBegin = n;
                    continue;
                }
            } else if (fmtStatus == 1) {
                if (m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmtBegin, n - fmtBegin - 2);
                    fmtStatus = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if (n == m_pattern.size()) {
                if (formatStr.empty()) {
                    formatStr = m_pattern.substr(i + 1);
                }
            }
        }
        if (fmtStatus == 0) {
            if (!abnormalStr.empty()) {
                vec.push_back(std::make_tuple(abnormalStr, std::string(), 0));
                abnormalStr.clear();
            }
            vec.push_back(std::make_tuple(formatStr, fmt, 1));
            i = n - 1;
        } else if (fmtStatus == 1) {
            // std::cout << "pattern parse error:" << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>", fmt, 0));
        }
    }
    if (!abnormalStr.empty()) {
        vec.push_back(std::make_tuple(abnormalStr, "", 0));
    }
}

void LogFormatter::ParseFormatItem(std::vector<std::tuple<std::string, std::string, int>> vec)
{
    // %m -- 消息体
    // %p -- level
    // %r -- 启动后的时间
    // %c -- 日志名称
    // %t -- 线程id
    // %n -- 回车换行
    // %d -- 时间
    // %f -- 文件名
    // %l -- 行号
    // %T -- table键
    // %F -- 协程号
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>> s_format_items = {
#define XX(str, FormatItemClass) \
    {#str, [](const std::string& fmt) { return FormatItem::ptr(new FormatItemClass(fmt));}}

    XX(m, MessageFormatItem),
    XX(p, LevelFormatItem),
    XX(r, ElapseFormatItem),
    XX(c, LogNameFormatItem),
    XX(t, ThreadIdFormatItem),
    XX(n, NewLineFormatItem),
    XX(d, DateTimeFormatItem),
    XX(f, FileNameFormatItem),
    XX(l, LineFormatItem),
    XX(T, TableFormatItem),
    XX(F, FiberIdFormatItem),
    XX(N, ThreadNameFormatItem),
#undef XX
    };
    for(auto& i : vec) {
        if(std::get<2>(i) == 0) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if (it == s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format" + std::get<0>(i) + ">>")));
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
        // std::cout << std::get<0>(i) << "--" << std::get<1>(i) << "--" << std::get<2>(i) << std::endl;
    }
}

LoggerManager::LoggerManager()
{
    m_root.reset(new Logger);
    m_root->AddAppender(LogAppender::ptr(new StdoutLogAppender()));
}

Logger::ptr LoggerManager::GetLogger(const std::string& name)
{
    auto it = m_loggermap.find(name);
    return it == m_loggermap.end() ? m_root : it->second;
}

void LoggerManager::init()
{
}

}