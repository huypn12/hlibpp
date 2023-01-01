#ifndef __TRACE_H__
#define __TRACE_H__

#include <fstream>
#include <string>

namespace org::mcss {
class trace {
public:

  virtual void FromStr(const std::string &) = 0;
  virtual std::string ToStr() = 0;

  virtual void Flush() = 0;
};
} // namespace org::mcss
#endif
