#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>

class System;

class Processor {
 private:
  int _number{-1};
  double _utilization{0.0};
  System *_sys{nullptr};

 public:
  /**
   * @brief Construct a new Processor object from a processor identifier and its related system 
   * 
   * @param number Processor identifier 
   * @param sys Address to related system
   */
  Processor(const int &number, System *sys) : _number{number}, _sys{sys} {};

  int number() const;
  double utilization() const;
  void setUtilization(const double &utilization);
};

#endif