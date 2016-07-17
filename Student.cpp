//
//  Student.cpp
//  netsocket
//
//  Created by Gloria Budiman on 7/16/16.
//  Copyright © 2016 gbudiman. All rights reserved.
//

#include <stdio.h>
#include "main.h"
#include "Student.h"
#include "StudentParser.hpp"

int main() {
  int32_t pid[NUM_STUDENTS];
  
  for (uint32_t p = 0; p < NUM_STUDENTS; p++) {
    pid[p] = fork();
    
    if (pid[p] < 0) {
      std::cerr << "Forking failed at iteration " << p << "\n";
    } else if (pid[p] == 0) {
      do_work(p + 1);
    }
  }


  return 0;
}

void do_work(uint32_t id) {
  if (PROJ_DEBUG) {
    std::cout << "Process " << ::getpid() << " created for student " << id << "\n";
  }
  
  new StudentParser(id);
  
  if (PROJ_DEBUG) {
    std::cout << "Process " << ::getpid() << " returned\n";
  }
  
  exit(0);
}
