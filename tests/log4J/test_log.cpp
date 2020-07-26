#include <iostream>
#include "../../src/base_framework/utility/utility.h"
#include "../../src/base_framework/log4J/log.h"



int main ()
{
#if 0
    // loggger使用：1-设置输出方式 2-设置LogEvent  3-调用logger::Log
    logging::Logger::ptr logger(new logging::Logger);
    logger->AddAppender(logging::LogAppender::ptr(new logging::StdoutLogAppender()));
    logging::LogEvent::ptr event(new logging::LogEvent(__FILE__, __LINE__, 1, utility::GetThreadId(), utility::GetFiberId(), time(0)));
    event->GetContentStream() << "Nihaowa123";
    logger->Log(logging::LogLevel::DEBUG, event);
#endif
    
    logging::Logger::ptr logger(new logging::Logger);
    logger->AddAppender(logging::LogAppender::ptr(new logging::StdoutLogAppender()));
    LOG_DEBUG(logger) << "xixihaha123";

    LOG_FMT_DEBUG(logger, "test macro fmt error %s", "aa");

    logging::FileLogAppender::ptr file_appender(new logging::FileLogAppender("./bma.log"));
    file_appender->SetLevel(logging::LogLevel::ERROR);
    logging::LogFormatter::ptr fmt(new logging::LogFormatter("%d%T%m%n"));

    file_appender->SetFormatter(fmt);
    logger->AddAppender(file_appender);
    LOG_FMT_ERROR(logger, "test fileLogAppender %s", "error xxxxx");
    LOG_FMT_INFO(logger, "test fileLogAppender %s", "info xxxxx");

    auto i = logging::loggerMgr::GetInstance()->GetLogger("xxxx");
    LOG_FMT_INFO(i, "-------%s--------", "Nihaowa");

    return 0;
}