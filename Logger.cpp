#include "Logger.h"

#include <cstdarg>
#include <iostream>

using namespace pv;

const std::string k_loggerType[ELoggerType::eLoggerCount] =
{
    "",
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR"
};


Logger::Logger()
    : _logFilename("logfile.log")
{
    _file.open(_logFilename, std::ofstream::out);
    _file.clear();
    _file.close();
}

Logger::~Logger()
{
    _file.close();
}

void Logger::createLogFile(const std::string& fileName)
{
    _logFilename = fileName;

    _file.open(_logFilename, std::ofstream::out);
    _file.clear();
    _file.close();
}

void Logger::log(ELoggerType type, ELogOut out, const char* format, ...)
{
    char buffer[256];

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    std::string message;
    message.assign(buffer);

    log(message, type, out);
}

void Logger::log(const std::string& message, ELoggerType type, ELogOut out)
{
    switch (out)
    {
    case ELogOut::eConsoleLog:
    {
        logToConsole(message, type);
        break;
    }

    case ELogOut::eFileLog:
    {
        logToFile(message, type);
        break;
    }
        
    case ELogOut::eBothLog:
    {
        logToConsole(message, type);
        logToFile(message, type);
        break;
    }

    default:
        break;
    }
}

void Logger::destroyLogFile()
{
    if (_file.is_open())
    {
        _file.close();
    }

    std::remove(_logFilename.c_str());
}

void Logger::logToConsole(const std::string& message, ELoggerType type)
{
    if (type == ELoggerType::eLoggerDebug)
    {
#if defined (_DEBUG) || defined(DEBUG)
        std::cout << k_loggerType[eLoggerDebug] << ": " << message << std::endl;
#endif
    }
    else
    {
        std::cout << k_loggerType[type] << ": " << message << std::endl;
    }
}

void Logger::logToFile(const std::string& message, ELoggerType type)
{
    _file.open(_logFilename, std::ofstream::out | std::ofstream::app);

    if (type == ELoggerType::eLoggerDebug)
    {
#if defined (_DEBUG) || defined(DEBUG)
        _file << k_loggerType[eLoggerDebug] << ": " << message << std::endl;
#endif
    }
    else
    {
        _file << k_loggerType[type] << ": " << message << std::endl;
    }

    _file.close();
}
