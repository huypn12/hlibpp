#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <chrono>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

// Super simple & stupid buffered logger
namespace org::mcss {
class Logger {
private:
  long window_size_;
  std::stringstream log_buf_;
  std::ofstream log_fptr_;

protected:
  long LogSize() {
    log_buf_.seekg(0, std::ios::end);
    int size = log_buf_.tellg();
    log_buf_.seekg(0, std::ios::end);
    return size;
  }

  void Log(const std::string &mesg) {
    log_buf_ << Now() << " : " << mesg << std::endl;
    auto log_size = LogSize();
    if (log_size > 512) {
      auto mesg = log_buf_.str();
      log_fptr_ << mesg;
      log_buf_.str(std::string()); // clear stream 
    }
  }

  std::string Now() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now), "%F %T%z");
    return ss.str();
  }

public:
  Logger(const std::string &log_fpath) { SetLogFile(log_fpath); }
  Logger() {};
  ~Logger() { Dump(); }

  void SetLogFile(const std::string &log_fpath) {
    log_fptr_.open(log_fpath, std::ios::out | std::ios::app);
    if (!log_fptr_.is_open()) {
      std::cerr << "[E] Unable to open log file, logging to stderr instead."
                << std::endl;
      log_fptr_.basic_ios<char>::rdbuf(std::cerr.rdbuf());
    }
  }

  void LogInfo(const std::string &mesg) { Log("[INFO] " + mesg); }

  void LogError(const std::string &mesg) { Log("[ERROR] " + mesg); }

  void Dump() { auto mesg = log_buf_.str(); log_fptr_ << mesg; }
};
} // namespace org::mcss

#endif
