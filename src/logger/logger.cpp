#include "logger.hpp"

#include <filesystem>
#include <ctime>

namespace
{
  std::string log_file_path;

  void
  create_log_file()
  {
    std::filesystem::path path(".");

    std::string time_stamp(::logger::get_time_stamp());
    std::replace(time_stamp.begin(), time_stamp.end(), ' ', '_');

    log_file_path = fmt::format("{:s}/{:s}_{:s}",
        std::filesystem::absolute(path).string(), time_stamp, ::logger::log_file_name);

    if (FILE* f_ptr = fopen(log_file_path.c_str(), "w"); f_ptr)
    {
      fclose(f_ptr);
    }
    else
    {
      log_file_path.clear();
    }
  }
}

std::string
logger::get_time_stamp()
{
  using Seconds = std::chrono::seconds;
  using Microseconds = std::chrono::microseconds;

  auto time_s = std::chrono::system_clock::now();
  time_t now = std::chrono::system_clock::to_time_t(time_s);
  tm* time_struct = localtime(&now);

  //YYYY-MM-DD (ISO 8601)
  return fmt::format("{:d<04}-{:d<02}-{:d<02} {:d<02}:{:d<02}:{:d<02}:{:d<06}",
                     1900 + time_struct->tm_year,
                     1 + time_struct->tm_mon,
                     time_struct->tm_mday,
                     time_struct->tm_hour,
                     time_struct->tm_min,
                     time_struct->tm_sec,
                     std::chrono::duration_cast<Microseconds>(time_s - std::chrono::floor<Seconds>(time_s)).count());
}

void
logger::save_log_to_file(const std::string& log)
{
  std::filesystem::path path(".");

  if(log_file_path.empty()) [[unlikely]]
  {
    create_log_file();
  }

  if (FILE* f_ptr = fopen(log_file_path.c_str(), "a"); f_ptr)
  {
    fmt::print(f_ptr, log);
    fclose(f_ptr);
  }
}
