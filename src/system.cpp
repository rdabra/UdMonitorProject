#include "system.h"

#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

/**
 * @brief Populates the vector of Processors
 * 
 * @return std::vector<Processor>& Processors
 */
std::vector<Processor> &System::Cpus() {
  std::vector<double> utilizations{LinuxParser::CpuUtilization()};
  _cpus.clear();

  int i{0};
  for (auto utilization : utilizations) {
    _cpus.emplace_back(i++,this);
    _cpus.back().setUtilization(utilization);
  }

  return _cpus;
}

vector<Process> &System::Processes() {
  std::vector<int> pids{LinuxParser::Pids()};
  _processes.clear();

  for (auto pid : pids) {
    _processes.emplace_back(pid,this);
    Process &p{_processes.back()};
    p.setCommand(LinuxParser::Command(pid));
    p.setCpuUtilization(LinuxParser::CpuUtilization(pid));
    p.setRam(LinuxParser::Ram(pid));
    p.setUpTime(LinuxParser::UpTime(pid));
    p.setUser(LinuxParser::User(pid));
  }

  return _processes;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

double System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

double System::UpTime() { return LinuxParser::UpTime(); }
