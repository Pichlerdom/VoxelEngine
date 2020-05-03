#ifndef _TIMER_H_
#define _TIMER_H_
#include <sys/time.h>

class Timer{
 public:
  void start(){
    gettimeofday(&last_tick_time, NULL);
  }
  
  double tick(){
    struct timeval tick_time;
    double time_elapsed;
    
    gettimeofday(&tick_time, NULL);
    
    time_elapsed = (tick_time.tv_sec - last_tick_time.tv_sec) * 1000.0f;
    time_elapsed += (tick_time.tv_usec - last_tick_time.tv_usec) /1000.0f;

    last_tick_time = tick_time;
    
    return time_elapsed;
  }
  
 private:
  struct timeval last_tick_time;
};
#endif
