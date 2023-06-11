#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "system.h"

int Process::pid() const { return _pid; }

std::string Process::user() const { return _user; }

void Process::setUser(const std::string &user) { _user = user; }

std::string Process::command() const { return _command; }

void Process::setCommand(const std::string &command) { _command = command; }

double Process::cpuUtilization() const { return _cpuUtilization; }

void Process::setCpuUtilization(const double &cpuUtilization) {
  _cpuUtilization = cpuUtilization;
}

std::string Process::ram() const { return _ram; }

void Process::setRam(const std::string &ram) { _ram = ram; }

double Process::upTime() const { return _upTime; }

void Process::setUpTime(double upTime) { _upTime = upTime; }

double Process::elapsedTime() const { return _sys->UpTime() - this->upTime(); };

bool Process::operator<(const Process &a) const {
  return this->cpuUtilization() < a.cpuUtilization();
}

bool Process::operator>(const Process &a) const {
  return this->cpuUtilization() > a.cpuUtilization();
}
