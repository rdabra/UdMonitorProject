#include "format.h"

#include <string>

using std::string;

string Format::ElapsedTime(double seconds) {
  unsigned long secs = (unsigned long)seconds;
  unsigned long hh = secs / 3600;
  unsigned long mm = (secs - hh * 3600) / 60;
  unsigned long ss = secs - hh * 3600 - mm * 60;

  auto ultostr{[](unsigned long a) -> std::string {
    return a < 10 ? "0" + std::to_string(a) : std::to_string(a);
  }};

  std::string resp{ultostr(hh) + ":" + ultostr(mm) + ":" + ultostr(ss)};
  return resp;
}