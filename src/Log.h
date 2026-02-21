#pragma once

#include <string_view>

enum class LogLevel
{
    Trace,
    Debug,
    Info,
    Warning,
    Error
};

class Logger
{
public:
    LogLevel m_level{LogLevel::Info};

public:
    void Log(LogLevel level, std::string_view message) const;
};

#define LTRACE(logger, message) logger.Log(LogLevel::Trace, message)
#define LDEBUG(logger, message) logger.Log(LogLevel::Debug, message)
#define LINFO(logger, message) logger.Log(LogLevel::Info, message)
#define LWARNING(logger, message) logger.Log(LogLevel::Warning, message)
#define LERROR(logger, message) logger.Log(LogLevel::Error, message)