#ifndef __TRACE_FILE_H__
#define __TRACE_FILE_H__

class TraceFile {
  static enum RetCode {
        FILE_OK = 0,
        FILE_ERR
  };
  static int Read(const std::string &fpath, int &alphabet_count,
                  std::string &trace_str) {
    auto ifs = std::ifstream(fpath);
    if (ifs.is_open()) {
      auto line = std::string("");
      int i = 0;
      while (std::getline(ifs, line)) {
        if (i == 0) {
          alphabet_count = std::stoi(line);
        } else if (i == 1) {
          trace_str = line;
        } else {
          break;
        }
        i++;
      }
      return FILE_OK;
    } else {
      return FILE_ERR;
    }
  }
};

#endif  // !__TRACE_FILE_H__
