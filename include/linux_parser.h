#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// Relevant Cpu Jiffies
struct CPU_JIFFIES {
  unsigned long user{0}, nice{0}, system{0}, idle{0}, iowait{0}, irq{0}, softirq{0},
      steal{0};
};

// System
double MemoryUtilization();
double UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// Helper funcions
CPU_JIFFIES fillCPUJiffies(std::string cpuDataLine);
std::vector<std::string> readCPUData(const std::string &fileName);

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};
std::vector<double> CpuUtilization();
double CpuUtilization(int pid);
unsigned long Jiffies(const CPU_JIFFIES &jif);
unsigned long ActiveJiffies(const CPU_JIFFIES &jif);
unsigned long IdleJiffies(const CPU_JIFFIES &jif);

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
double UpTime(int pid);
};  // namespace LinuxParser

#endif