#ifndef _PV_LOGGER_H_
#define _PV_LOGGER_H_

#include "Singleton.h"

#include "common.h"

namespace pv
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    enum ELoggerType
    {
        eLoggerNone,
        eLoggerDebug,
        eLoggerInfo,
        eLoggerWarning,
        eLoggerError,

        eLoggerCount
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class ELogOut
    {
        eConsoleLog,
        eFileLog,
        eBothLog
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    class Logger : public Singleton<Logger>
    {
    public:
        Logger();
        virtual         ~Logger();

        void            createLogFile(const std::string& filename);

        void            log(const std::string& message, ELoggerType type = ELoggerType::eLoggerInfo, ELogOut out = ELogOut::eConsoleLog);
        void            log(ELoggerType type, ELogOut out, const char* format, ...);

        void            destroyLogFile();

    protected:

        void            logToConsole(const std::string& message, ELoggerType type);
        void            logToFile(const std::string& message, ELoggerType type);

        std::string     _logFilename;
        std::ofstream   _file;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////

#if USE_LOGGER
#   define LOG(messages, ...) Logger::getInstance()->log(pv::ELoggerType::eLoggerNone, pv::ELogOut::eConsoleLog, messages, ##__VA_ARGS__);
#   define LOG_GEBUG(messages, ...) Logger::getInstance()->log(pv::ELoggerType::eLoggerDebug, pv::ELogOut::eConsoleLog, messages, ##__VA_ARGS__);
#   define LOG_INFO(messages, ...) Logger::getInstance()->log(pv::ELoggerType::eLoggerInfo, pv::ELogOut::eConsoleLog, messages, ##__VA_ARGS__);
#   define LOG_WARNING(messages, ...) Logger::getInstance()->log(pv::ELoggerType::eLoggerWarning, pv::ELogOut::eConsoleLog, messages, ##__VA_ARGS__);
#   define LOG_ERROR(messages, ...) Logger::getInstance()->log(pv::ELoggerType::eLoggerError, pv::ELogOut::eConsoleLog, messages, ##__VA_ARGS__);
#else
#   define LOG(messages, ...)
#   define LOG_GEBUG(messages, ...)
#   define LOG_INFO(messages, ...)
#   define LOG_WARNING(messages, ...)
#   define LOG_ERROR(messages, ...)
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////
}
#endif //_PV_LOGGER_H_
