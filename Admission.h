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
#include "Database.hpp"

void create_empty_database_file();
void create_tcp_and_process();
void make_admission_decision();

std::string debug_receive_buffer(char*, int);
uint32_t process_department_message(std::string);
uint32_t process_student_message(std::string);

bool check_department_completion(int*, int*);

int handle_department_messages(int, const char*, char*);
int handle_student_messages(int, std::string, char*);
AdmissionMesseger *am = new AdmissionMesseger();

std::map<std::string, float> *database;
std::vector<std::string> *interest_database;
float student_gpa;

bool build_database();

char receive_buffer[MAXDATASIZE];
int receive_length;
Database *db;
#endif /* Admission_h */
