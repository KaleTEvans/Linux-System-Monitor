#include "processor.h"
#include "linux_parser.h"

// NOTE: This processor utilization is measured over the time since system startup 
//       until current time. To get a more accurate reading, it is suggested to gather data for
//       two different time periods and use the difference to calculate utilization
float Processor::Utilization() { 
    long TotalUse = GetTotal();
    long Idle = GetIdle();

    float utilization = (float(TotalUse) - float(Idle)) / float(TotalUse);
    return utilization;
}

long Processor::GetTotal() { return LinuxParser::Jiffies(); }
long Processor::GetIdle() { return LinuxParser::IdleJiffies(); }
long Processor::GetNonIdle() { return LinuxParser::ActiveJiffies(); }