//
//  Timer.hpp
//  RGAssignment6
//
//  Created by rick gessner on 5/17/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Timer_h
#define Timer_h

#include <ctime>
#include <chrono>

class Timer {
public:
  Timer() {
    stopped=started=std::chrono::high_resolution_clock::now();
  };
  
  Timer& start() {
    started=std::chrono::high_resolution_clock::now();
    return *this;
  }
  
  Timer& stop() {
    stopped=std::chrono::high_resolution_clock::now();
    return *this;
  }
  
  double elapsed() {
    if(started!=stopped) {
      std::chrono::duration<double> elapsed = stopped - started;
      return elapsed.count(); //move to SEC range?
    }
    return 0.0;
  }
  
  std::chrono::time_point<std::chrono::high_resolution_clock> started;
  std::chrono::time_point<std::chrono::high_resolution_clock> stopped;
};

#endif /* Timer_h */
