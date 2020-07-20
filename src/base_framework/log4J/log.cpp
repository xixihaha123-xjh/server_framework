// 建议按照稳定度排序：
// cpp 对应的头文件, C/C++标准库, 系统库的.h, 其他库的.h, 本项目内其他的.h
#include "log.h"

#include <sstream>
#include <iostream>

namespace log {

Logger::Logger(const std::string& name) : m_name(name)
{
}

void Logger::AddAppender(LogAppender::ptr appender)
{
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
    if (level >= m_level) {
        for (auto &i : m_appenders) {
            i->Log(level, event);
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

void StdoutLogAppender::Log(LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= m_level) {
        std::cout << m_formatter.Format(event);
    }

}

FileLogAppender::FileLogAppender(const std::string& filename) : m_filename(filename)
{
}

bool FileLogAppender::Reopen()
{
    if (m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return true;
}

void FileLogAppender::Log(LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= m_level) {
        m_filestream << m_formatter.Format(event);
    }
}

LogFormatter::LogFormatter(const std::string& pattern) : m_pattern(pattern)
{
}

std::string LogFormatter::Format(LogEvent::ptr event)
{
    std::stringstream ss;
    for (auto& i : m_items) {
        i->Format(ss, event);
    }
    return ss.str();
}

void LogFormatter::Init()
{
    // str  format  type
    // m_pattern: %d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n
    // format 
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string normalStr;
    for (size_t i = 0; i < m_pattern.size(); ++i) {
        if (m_pattern[i] != '%') {
            normalStr.append(1, m_pattern[i]);
            continue;
        }

        if ((i + 1) < m_pattern.size()) {
            if (m_pattern[i + 1] == '%') {
                normalStr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmtStatus = 0;   // 解析状态,为0解析正常,为1解析不支持
        size_t fmtBegin = 0; // 取出时间str

        std::string str;
        std::string fmt;
        while (n < m_pattern.size()) {
            if (!fmtStatus && (!isspace(m_pattern[n]))) {
                str = 
                break;
            }
            if (fmtStatus == 0) {
                if (m_pattern[n] == '{') {

                }
            }
        }
        
    }
}

}