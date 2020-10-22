#include <stdarg.h>
#include <mutex>
#include <vector>

#include "Log.h"
#include "spdlog/sinks/daily_file_sink.h"

namespace XSPlayer {
static std::shared_ptr<Log> g_log;
static std::mutex g_logMutex;

void Log::SetLogWriteCallback(LogWriteCallback callback) {
  m_logWirateCallback = callback;
}

void Log::Write(LogLevel logLevel, const std::string& log) {
  switch (logLevel) {
    case LOG_WARNING_MESSAGE:
      m_logger->warn(log);
      break;
    case LOG_DEBUG_MESSAGE:
      m_logger->debug(log);
      break;
    case LOG_ERROR_MESSAGE:
      m_logger->error(log);
      break;
    case LOG_INFO_MESSAGE:
      m_logger->info(log);
      break;
    default:
      break;
  }

  if (nullptr != m_logWirateCallback) m_logWirateCallback(logLevel, log);
}


void Log::Write(LogLevel logLevel, const char* log, size_t len) {
  std::string strLog(log, len);
  Write(logLevel, strLog);
}

std::shared_ptr<Log> Log::GetInstance(void) {
  if (nullptr == g_log) {
    std::unique_lock<std::mutex> lock(g_logMutex);
    if (nullptr == g_log) {
      g_log = std::shared_ptr<Log>(new Log);
    }
  }
  return g_log;
}

Log::Log() {
  std::vector<spdlog::sink_ptr> sinks;
#ifdef _CONSOLE
  sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
#endif
  sinks.push_back(
      std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/log", 0, 0));
  m_logger = std::make_shared<spdlog::logger>("both", begin(sinks), end(sinks));
  spdlog::register_logger(m_logger);

#ifdef _DEBUG
  m_logger->set_level(spdlog::level::trace);
#else
  m_logger->set_level(spdlog::level::err);
#endif
  m_logger->set_pattern("%T.%e  %t  [%L]  %v");
  m_logger->flush_on(spdlog::level::err);
}

Log::~Log() {}

}  // namespace XSPlayer
