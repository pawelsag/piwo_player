#pragma once

#include <QString>
#include <chrono>
#include <fstream>
#include <string>

#include <fmt/color.h>
#include <fmt/format.h>

#include "logger_config.hpp"

namespace logger
{

#ifdef LOGGER_LOG_TO_FILE
  constexpr bool log_to_file = true;
#else
  constexpr bool log_to_file = false;
#endif

#ifdef LOGGER_LOG_TO_CONSOLE
  constexpr bool log_to_console = true;
#else
  constexpr bool log_to_console = false;
#endif

#ifdef LOGGING_LEVEL
  constexpr uint32_t logging_level = LOGGING_LEVEL;
#else
  constexpr uint32_t logging_level = 0;
#endif

constexpr const char* log_file_name = "piwo_player.log";

enum class log_type
{
  INFO = 0,
  WARNING = 1,
  ERROR = 2
};

enum class log_level
{
  MAJOR = 0,
  MINOR = 1,
  NOISY = 2
};

std::string
get_time_stamp();

template<log_level log_level_value, log_type log_type_value, bool fetch_time>
void
log(const char*, const char*, const int, const std::string&);

void
save_log_to_file(const std::string&);

template<typename T>
std::string
log_arg_to_string(const T);

template<typename... Args>
std::string
log_arguments_to_string(Args...);

template<typename... Args>
std::string
fmt_format_log_arguments(const std::string&, Args&&...);

std::string
get_time_stamp();

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef SHOW_LOGS
#define LOG(LOG_LEVEL, LOG_TYPE, ...)                                                                                  \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::LOG_TYPE, false>(                                \
      __FILENAME__, __func__, __LINE__, ::logger::log_arguments_to_string(__VA_ARGS__));                               \
  }
#define LOG_INFO(LOG_LEVEL, ...)                                                                                       \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::INFO, false>(                                    \
      __FILENAME__, __func__, __LINE__, ::logger::log_arguments_to_string(__VA_ARGS__));                               \
  }
#define LOG_WARNING(LOG_LEVEL, ...)                                                                                    \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::WARNING, false>(                                 \
      __FILENAME__, __func__, __LINE__, ::logger::log_arguments_to_string(__VA_ARGS__));                               \
  }
#define LOG_ERROR(LOG_LEVEL, ...)                                                                                      \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::ERROR, false>(                                   \
      __FILENAME__, __func__, __LINE__, ::logger::log_arguments_to_string(__VA_ARGS__));                               \
  }
#define TLOG(LOG_LEVEL, LOG_TYPE, ...)                                                                                 \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::LOG_TYPE, true>(                                 \
      __FILENAME__, __func__, __LINE__, ::logger::log_arguments_to_string(__VA_ARGS__));                               \
  }
#define TLOG_INFO(LOG_LEVEL, ...)                                                                                      \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::INFO, true>(                                     \
      __FILENAME__, __func__, __LINE__, ::logger::log_arguments_to_string(__VA_ARGS__));                               \
  }
#define TLOG_WARNING(LOG_LEVEL, ...)                                                                                   \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::WARNING, true>(                                  \
      __FILENAME__, __func__, __LINE__, ::logger::log_arguments_to_string(__VA_ARGS__));                               \
  }
#define TLOG_ERROR(LOG_LEVEL, ...)                                                                                     \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::ERROR, true>(                                    \
      __FILENAME__, __func__, __LINE__, ::logger::log_arguments_to_string(__VA_ARGS__));                               \
  }
#define FLOG(LOG_LEVEL, LOG_TYPE, MSG_FORMAT, ...)                                                                     \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::LOG_TYPE, false>(                                \
      __FILENAME__, __func__, __LINE__, ::logger::fmt_format_log_arguments((MSG_FORMAT), __VA_ARGS__));                \
  }
#define FLOG_INFO(LOG_LEVEL, MSG_FORMAT, ...)                                                                          \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::INFO, false>(                                    \
      __FILENAME__, __func__, __LINE__, ::logger::fmt_format_log_arguments((MSG_FORMAT), __VA_ARGS__));                \
  }
#define FLOG_WARNING(LOG_LEVEL, MSG_FORMAT, ...)                                                                       \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::WARNING, false>(                                 \
      __FILENAME__, __func__, __LINE__, ::logger::fmt_format_log_arguments((MSG_FORMAT), __VA_ARGS__));                \
  }
#define FLOG_ERROR(LOG_LEVEL, MSG_FORMAT, ...)                                                                         \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::ERROR, false>(                                   \
      __FILENAME__, __func__, __LINE__, ::logger::fmt_format_log_arguments((MSG_FORMAT), __VA_ARGS__));                \
  }
#define FTLOG(LOG_LEVEL, LOG_TYPE, MSG_FORMAT, ...)                                                                    \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::LOG_TYPE, true>(                                 \
      __FILENAME__, __func__, __LINE__, ::logger::fmt_format_log_arguments((MSG_FORMAT), __VA_ARGS__));                \
  }
#define FTLOG_INFO(LOG_LEVEL, MSG_FORMAT, ...)                                                                         \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::INFO, true>(                                     \
      __FILENAME__, __func__, __LINE__, ::logger::fmt_format_log_arguments((MSG_FORMAT), __VA_ARGS__));                \
  }
#define FTLOG_WARNING(LOG_LEVEL, MSG_FORMAT, ...)                                                                      \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::WARNING, true>(                                  \
      __FILENAME__, __func__, __LINE__, ::logger::fmt_format_log_arguments((MSG_FORMAT), __VA_ARGS__));                \
  }
#define FTLOG_ERROR(LOG_LEVEL, MSG_FORMAT, ...)                                                                        \
  {                                                                                                                    \
    ::logger::log<::logger::log_level::LOG_LEVEL, ::logger::log_type::ERROR, true>(                                    \
      __FILENAME__, __func__, __LINE__, ::logger::fmt_format_log_arguments((MSG_FORMAT), __VA_ARGS__));                \
  }
#else

#define LOG(LOG_LEVEL, LOG_TYPE, ...)
#define LOG_INFO(LOG_LEVEL, ...)
#define LOG_WARNING(LOG_LEVEL, ...)
#define LOG_ERROR(LOG_LEVEL, ...)
#define TLOG(LOG_LEVEL, LOG_TYPE, ...)
#define TLOG_INFO(LOG_LEVEL, ...)
#define TLOG_WARNING(LOG_LEVEL, ...)
#define TLOG_ERROR(LOG_LEVEL, ...)
#define FLOG(LOG_LEVEL, LOG_TYPE, MSG_FORMAT, ...)
#define FLOG_INFO(LOG_LEVEL, MSG_FORMAT, ...)
#define FLOG_WARNING(LOG_LEVEL, MSG_FORMAT, ...)
#define FLOG_ERROR(LOG_LEVEL, MSG_FORMAT, ...)
#define FTLOG(LOG_LEVEL, LOG_TYPE, MSG_FORMAT, ...)
#define FTLOG_INFO(LOG_LEVEL, MSG_FORMAT, ...)
#define FTLOG_WARNING(LOG_LEVEL, MSG_FORMAT, ...)
#define FTLOG_ERROR(LOG_LEVEL, MSG_FORMAT, ...)

#endif
}

template<logger::log_level log_level_value, logger::log_type log_type_value, bool fetch_time>
void
logger::log(const char* filename, const char* function, const int line, const std::string& message)
{
  if constexpr (static_cast<uint32_t>(log_level_value) <= logging_level)
  {
    std::string time_stamp;
    std::string log;
    if constexpr (fetch_time) {
      time_stamp = std::move(get_time_stamp().append(" "));
    }

    if constexpr (log_type_value == log_type::INFO)
    {
      log = fmt::format("{}INFO: [ {}:{} {}() ] {}\n", time_stamp, filename, line, function, message);
    }
    else if constexpr (log_type_value == log_type::WARNING)
    {
      log = fmt::format(fg(fmt::terminal_color::yellow),
                        "{}WARNING: [ {}:{} {}() ] {}\n",
                        time_stamp,
                        filename,
                        line,
                        function,
                        message);
    }
    else if constexpr (log_type_value == log_type::ERROR)
    {
      log = fmt::format(fg(fmt::terminal_color::red),
                        "{}ERROR: [ {}:{} {}() ] {} Reason: {} (errno: {})\n",
                        time_stamp,
                        filename,
                        line,
                        function,
                        message,
                        std::strerror(errno),
                        errno);
      errno = 0;
    }

    if constexpr (log_to_file)
    {
      save_log_to_file(log);
    }

    if constexpr (log_to_console)
    {
      fmt::print(log);
    }
  }
}

template<typename T>
std::string
logger::log_arg_to_string(const T val)
{
  if constexpr (std::is_arithmetic<T>::value)
  {
    return std::to_string(val);
  }
  else if constexpr (std::is_same<QString, T>::value)
  {
    return val.QString::toStdString();
  }
  else if constexpr (std::is_same<const char*, T>::value || std::is_same<char*, T>::value)
  {
    return std::string(val);
  }
  else if constexpr (std::is_same<std::string, T>::value)
  {
    return val;
  }
}

template<typename... Args>
std::string
logger::log_arguments_to_string(Args... args)
{
  std::string out;

  (out += ... += log_arg_to_string<decltype(args)>(args));

  return out;
}

template<typename... Args>
std::string
logger::fmt_format_log_arguments(const std::string& format, Args&&... args)
{
  return fmt::format(format, std::forward<Args>(args)...);
}
