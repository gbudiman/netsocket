//
//  Admission.h
//  netsocket
//
//  Created by Gloria Budiman on 7/16/16.
//  Copyright © 2016 gbudiman. All rights reserved.
//

#ifndef Admission_h
#define Admission_h

#define CLIENT_IS_DEPARTMENT 1
#define CLIENT_IS_STUDENT 2

#include "main.h"
#include "AdmissionMessenger.hpp"
#include "Socket.hpp"

std::string debug_receive_buffer(char*, int);
uint32_t process_department_message(char*, int, std::map<std::string, float>*);

void check_department_completion(int*);

int handle_department_messages(int, const char*, char*);
int handle_student_messages(int, const char*, char*);
AdmissionMesseger *am = new AdmissionMesseger();

std::map<std::string, float> *database;
char receive_buffer[MAXDATASIZE];
int receive_length;
#endif /* Admission_h */
