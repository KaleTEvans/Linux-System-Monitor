#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  
  long GetTotal();
  long GetIdle();
  long GetNonIdle();

  long NonIdle;
  long Idle;
  long TotalUse;
};

#endif