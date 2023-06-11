#include "processor.h"

int Processor::number() const
{
    return _number;
}

double Processor::utilization() const
{
    return _utilization;
}

void Processor::setUtilization(const double &utilization){
    _utilization = utilization;   
}