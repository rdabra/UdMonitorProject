#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR *directory = opendir(kProcDirectory.c_str());
  struct dirent *file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

double LinuxParser::MemoryUtilization() {
  std::ifstream f{kProcDirectory + kMeminfoFilename};
  std::string line{""};
  std::string key{""};
  std::string value{""};
  double MemTotal{0};
  double MemFree{0};
  double Buffers{0};
  double Cached{0};
  double resp{0.};

  if (f.is_open()) {
    while (std::getline(f, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream{line};
      lineStream >> key >> value;
      if (key == "MemTotal")
        MemTotal = std::stod(value);
      else if (key == "MemFree")
        MemFree = std::stod(value);
      else if (key == "Buffers")
        Buffers = std::stod(value);
      else if (key == "Cached") {
        Cached = std::stod(value);
        break;
      }
    }
    f.close();
    resp = (MemTotal - MemFree - Buffers - Cached) / MemTotal;
  }

  return resp;
}

double LinuxParser::UpTime() {
  std::ifstream f{kProcDirectory + kUptimeFilename};
  std::string line{""};
  std::string value{""};
  double upTime{0};

  if (f.is_open()) {
    std::getline(f, line);
    std::replace(line.begin(), line.end(), '.', ' ');
    std::istringstream lineStream{line};
    lineStream >> value;
    upTime = std::stod(value);
    f.close();
  }
  return upTime;
}

std::vector<std::string> LinuxParser::readCPUData(const std::string &fileName) {
  std::vector<std::string> resp;
  std::ifstream f{fileName};
  std::string line{""};
  if (f.is_open()) {
    while (std::getline(f, line))
      if (line.find("cpu") != std::string::npos)
        resp.emplace_back(line);
      else
        break;
    f.close();
  }
  return resp;
}

LinuxParser::CPU_JIFFIES LinuxParser::fillCPUJiffies(std::string cpuDataLine) {
  std::string v{""}, v1{""}, v2{""}, v3{""}, v4{""}, v5{""}, v6{""}, v7{""},
      v8{""};
  CPU_JIFFIES resp;

  std::istringstream lineStream{cpuDataLine};
  lineStream >> v >> v1 >> v2 >> v3 >> v4 >> v5 >> v6 >> v7 >> v8;
  resp.user = std::stoul(v1);
  resp.nice = std::stoul(v2);
  resp.system = std::stoul(v3);
  resp.idle = std::stoul(v4);
  resp.iowait = std::stoul(v5);
  resp.irq = std::stoul(v6);
  resp.softirq = std::stoul(v7);
  resp.steal = std::stoul(v8);

  return resp;
}

unsigned long LinuxParser::ActiveJiffies(const CPU_JIFFIES &jif) {
  unsigned long NonIdle{jif.user + jif.nice + jif.system + jif.irq +
                        jif.softirq + jif.steal};

  return NonIdle;
}

unsigned long LinuxParser::IdleJiffies(const CPU_JIFFIES &jif) {
  unsigned long Idle{jif.idle + jif.iowait};

  return Idle;
}

unsigned long LinuxParser::Jiffies(const CPU_JIFFIES &jif) {
  unsigned long resp{IdleJiffies(jif) + ActiveJiffies(jif)};

  return resp;
}

/**
 * @brief Calculates all cpus utilizations based on the first answer of  
 * https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
 * 
 * @return std::vector<double> Vector of cpus utilizations (aggregate utilization at index 0)
 */
std::vector<double> LinuxParser::CpuUtilization() {
  std::vector<std::string> cpuData{readCPUData(kProcDirectory + kStatFilename)};
  std::vector<double> resp;
  for (unsigned i{0}; i < cpuData.size(); i++) {
    CPU_JIFFIES jif{fillCPUJiffies(cpuData[i])};
    double active{(double)ActiveJiffies(jif)};
    double total{(double)Jiffies(jif)};
    resp.emplace_back(active / total);
  }

  return resp;
}

int LinuxParser::TotalProcesses() {
  std::ifstream f{kProcDirectory + kStatFilename};
  std::string line{""};
  std::string key{""};
  std::string value{""};
  int resp{0};
  if (f.is_open()) {
    while (std::getline(f, line)) {
      std::istringstream lineStream{line};
      lineStream >> key >> value;
      if (key == "processes") break;
    }
    f.close();
    resp = std::stoi(value);
  }

  return resp;
}

int LinuxParser::RunningProcesses() {
  std::ifstream f{kProcDirectory + kStatFilename};
  std::string line{""};
  std::string key{""};
  std::string value{"0"};
  int resp{0};
  if (f.is_open()) {
    while (std::getline(f, line)) {
      std::istringstream lineStream{line};
      lineStream >> key >> value;
      if (key == "procs_running") break;
    }
    f.close();
    resp = std::stoi(value);
  }
  return resp;
}

/**
 * @brief Calculates process cpu utilization based on the first answer of 
 * https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599t
 * 
 * @param pid Process identifier
 * @return double Cpu utilization 
 */
double LinuxParser::CpuUtilization(int pid) {
  std::ifstream f{kProcDirectory + std::to_string(pid) + kStatFilename};
  std::string line{""};
  std::string value{""};
  double utime{0.}, stime{0.}, cutime{0.}, cstime{0.}, starttime{0.};
  double resp{0.};
  double uptime{LinuxParser::UpTime()};
  if (f.is_open()) {
    if (std::getline(f, line)) {
      std::istringstream lineStream{line};
      int i{0};
      do {
        lineStream >> value;
        switch (i) {
          case 13:
            utime = std::stod(value);
            break;
          case 14:
            stime = std::stod(value);
            break;
          case 15:
            cutime = std::stod(value);
            break;
          case 16:
            cstime = std::stod(value);
            break;
          case 21:
            starttime = std::stod(value);
            break;
        }
      } while (++i < 22);
      double totalTime{utime + stime + cutime + cstime};
      double clockTicks{(double)sysconf(_SC_CLK_TCK)};
      double seconds{uptime - (starttime / clockTicks)};
      double utilization{seconds > 0. ? 100. * totalTime / (clockTicks * seconds) : 0.};
      resp = utilization;
    }
    f.close();
  };
  return resp;
}

string LinuxParser::Command(int pid) {
  std::ifstream f{kProcDirectory + std::to_string(pid) + kCmdlineFilename};
  std::string line{""};
  std::string value{""};
  if (f.is_open()) {
    if (std::getline(f, line)) {
      std::istringstream lineStream{line};
      lineStream >> value;
    }
    f.close();
  }

  return value;
}

string LinuxParser::Ram(int pid) {
  std::ifstream f{kProcDirectory + std::to_string(pid) + kStatusFilename};
  std::string line{""};
  std::string key{""};
  std::string value{""};
  std::string resp{"0"};
  if (f.is_open()) {
    while (std::getline(f, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream{line};
      lineStream >> key >> value;
      if (key == "VmSize") break;
    }
    f.close();
    resp = std::to_string(std::stod(value) / 1024.);
  }

  return resp;
}

std::string LinuxParser::Uid(int pid) {
  std::ifstream f{kProcDirectory + std::to_string(pid) + kStatusFilename};
  std::string line{""};
  std::string key{""};
  std::string value{""};
  if (f.is_open()) {
    while (std::getline(f, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream{line};
      lineStream >> key >> value;
      if (key == "Uid") break;
    }
    f.close();
  }

  return value;
}

std::string LinuxParser::User(int pid) {
  std::string uId{Uid(pid)};
  std::ifstream f{kPasswordPath};
  std::string line{""};
  std::string key{""};
  std::string v1, v2{""};
  if (f.is_open()) {
    while (std::getline(f, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream{line};
      lineStream >> key >> v1 >> v2;
      if (v2 == uId) break;
    }
    f.close();
  }

  return key;
}

double LinuxParser::UpTime(int pid) {
  std::ifstream f{kProcDirectory + std::to_string(pid) + kStatFilename};
  std::string line{""};
  std::string value{""};
  double resp{0.};
  if (f.is_open()) {
    if (std::getline(f, line)) {
      std::istringstream lineStream{line};
      int i{0};
      do {
        lineStream >> value;
      } while (++i < 22);
      resp = std::stod(value) / (double)sysconf(_SC_CLK_TCK);
    }
    f.close();
  }
  return resp;
}
