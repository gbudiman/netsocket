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
#include "AdmissionMessenger.hpp"

std::string debug_receive_buffer(char*, int);
uint32_t process_department_message(char*, int, std::map<std::string, float>*);

void check_department_completion(int*);
std::string get_client_ip_address(int);
std::string get_all_addresses();

AdmissionMesseger *am = new AdmissionMesseger();
#endif /* Admission_h */
