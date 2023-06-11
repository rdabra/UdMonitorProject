#ifndef PROCESS_H
#define PROCESS_H


#include <string>

class System;

class Process
{
  private:
    int _pid;
    std::string _user{""};
    std::string _command{""};
    double _cpuUtilization{0.0};
    std::string _ram{""};
    double _upTime{0};
    System *_sys{nullptr};

  public:
    /**
     * @brief Construct a new Process object from a process identifier and the related system
     * 
     * @param pid Processe identifier
     * @param sys Address to a system
     */
    Process(int pid, System *sys) : _pid{pid}, _sys{sys} {};
    int pid() const;
    std::string user() const;
    void setUser(const std::string &user);
    std::string command() const;
    void setCommand(const std::string &command);
    double cpuUtilization() const;
    void setCpuUtilization(const double &cpuUtilization);
    std::string ram() const;
    void setRam(const std::string &ram);
    double upTime() const;
    void setUpTime(double upTime);
    double elapsedTime() const;
    bool operator<(const Process &a) const;
    bool operator>(const Process &a) const;
};


#endif