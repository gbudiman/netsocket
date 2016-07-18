//
//  Admission.h
//  netsocket
//
//  Created by Gloria Budiman on 7/16/16.
//  Copyright © 2016 gbudiman. All rights reserved.
//

#ifndef Admission_h
#define Admission_h

#include "main.h"

#define AMSG_P1_START 0
#define AMSG_DEPT_COMPLETED 1
#define AMSG_P1_END 2
#define AMSG_P2_START 3

std::string debug_receive_buffer(char*, int);
uint32_t process_department_message(char*, int, std::map<std::string, float>*);

void fm_self_tcp_ip(addrinfo*, char*);
void fm_dept_completed(char);
void fm_phase1_completed();
void flow_message(int, std::vector<std::string>*);
void check_department_completion(int*);
#endif /* Admission_h */
