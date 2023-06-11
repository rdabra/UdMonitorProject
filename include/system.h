#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
  public:
    std::vector<Processor> &Cpus();
    std::vector<Process> &Processes();
    double MemoryUtilization();
    double UpTime();
    int TotalProcesses();
    int RunningProcesses();
    std::string Kernel();
    std::string OperatingSystem();

    // TODO: Define any necessary private members
  private:
    std::vector<Processor> _cpus = {};
    std::vector<Process> _processes = {};
};

#endif