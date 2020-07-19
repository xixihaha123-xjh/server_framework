#include "log.h"

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

}

void Logger::Debug(LogEvent::ptr event)
{

}

void Logger::Info(LogEvent::ptr event)
{

}

void Logger::Warn(LogEvent::ptr event)
{

}

void Logger::Error(LogEvent::ptr event)
{

}

void Logger::Fatal(LogEvent::ptr event)
{

}

}