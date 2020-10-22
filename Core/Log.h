//  Copyright 2020 TAL
#ifndef UTIL_LOG_H
#define UTIL_LOG_H

#include <functional>
#include <memory>
#include <string>
#include "spdlog/spdlog.h"

namespace XSPlayer {

using LogWriteCallback = std::function<void(int, const std::string&)>;

class Log {
public:
    enum LogLevel {
        LOG_DEBUG_MESSAGE = 0,
        LOG_INFO_MESSAGE,
        LOG_WARNING_MESSAGE,
        LOG_ERROR_MESSAGE,
        LOG_CRITICAL_MESSAGE,
    };
public:
    ~Log();
    void SetLogWriteCallback(LogWriteCallback callback);
    template <typename... Args>
    void Write(LogLevel logLevel, const std::string& fromat,
             const Args&... args) {
    spdlog::memory_buf_t buf;
    fmt::format_to(buf, fromat, args...);

    Write(logLevel, buf.data(), buf.size());
    }
    void Write(LogLevel logLevel, const std::string& log);
    static std::shared_ptr<Log> GetInstance(void);

protected:
    void Write(LogLevel logLevel, const char* log, size_t len);

private:
    Log();
    LogWriteCallback m_logWirateCallback;
    std::shared_ptr<spdlog::logger> m_logger;
};
}  // namespace XSPlayer

#define LOGD(format, ...)                                               \
  XSPlayer::Log::GetInstance()->Write(XSPlayer::Log::LOG_DEBUG_MESSAGE, "FILE:{}, FUNCTION:{},"##format, \
                            __FILE__, __FUNCTION__, ##__VA_ARGS__)
#define LOGE(format, ...)                                               \
  XSPlayer::Log::GetInstance()->Write(XSPlayer::Log::LOG_ERROR_MESSAGE, "FILE:{}, FUNCTION:{},"##format, \
                            __FILE__, __FUNCTION__, ##__VA_ARGS__)
#define LOGW(format, ...)                                               \
  XSPlayer::Log::GetInstance()->Write(XSPlayer::Log::LOG_WARNING_MESSAGE, "FILE:{}, FUNCTION:{},"##format, \
                            __FILE__, __FUNCTION__, ##__VA_ARGS__)
#define LOGI(format, ...)                                               \
  XSPlayer::Log::GetInstance()->Write(XSPlayer::Log::LOG_INFO_MESSAGE, "FILE:{}, FUNCTION:{},"##format, \
                            __FILE__, __FUNCTION__, ##__VA_ARGS__)
#define LOGC(format, ...)                                             \
  XSPlayer::Log::GetInstance()->Write(XSPlayer::Log::LOG_CRITICAL_MESSAGE, "FILE:{}, FUNCTION:{},"##format, \
                            __FILE__, __FUNCTION__, ##__VA_ARGS__)

#define LOG_ENTER LOGI("enter")
#define LOG_LIVE LOGI("live")

#endif
