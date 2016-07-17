//
//  Department.cpp
//  netsocket
//
//  Created by Gloria Budiman on 7/16/16.
//  Copyright © 2016 gbudiman. All rights reserved.
//

#include <stdio.h>
#include "main.h"
#include "Department.h"
#include "DepartmentParser.hpp"

int main() {
  int32_t pid[NUM_DEPTS];
  
  for (uint32_t p = 0; p < NUM_DEPTS; p++) {
    pid[p] = fork();
    
    if (pid[p] < 0) {
      std::cerr << "Forking failed at iteration " << p << "\n";
    } else if (pid[p] == 0) {
      do_work(p + 0x41);
    }
  }
  
  return 0;
}

void do_work(char d) {
  if (PROJ_DEBUG) {
    std::cout << "Process " << ::getpid() << " created for department " << d << "\n";
  }
  
  std::string dept_name = "";
  dept_name += d;
  new DepartmentParser(dept_name);
  
  if (PROJ_DEBUG) {
    std::cout << "Process " << ::getpid() << " returned\n";
  }
  
  exit(0);
}
