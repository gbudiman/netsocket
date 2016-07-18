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

std::string debug_receive_buffer(char*, int);
uint32_t process_department_message(char*, int, std::map<std::string, float>*);
#endif /* Admission_h */
