//
//  Department.h
//  netsocket
//
//  Created by Gloria Budiman on 7/16/16.
//  Copyright © 2016 gbudiman. All rights reserved.
//

#ifndef Department_h
#define Department_h
#include "DepartmentMessenger.hpp"
#include "DepartmentParser.hpp"

int do_work(char);
int connect_to_admission_server(DepartmentParser*, char);
int send_data_to_admission_server(char, int, DepartmentParser*);
void spawn_recursive(int);
void spawn_iterative(int);
void spawn_one();
void get_self_interfaces_info();

void flow_message(int, std::vector<std::string>*);
void fm_self_tcp_ip(char, addrinfo*, char*);
void fm_self_connected(char);
void fm_self_program_sent(char, std::string);
void fm_self_all_program_sent(char);
void fm_self_phase1_done(char);

DepartmentMessenger *dm = new DepartmentMessenger();

#endif /* Department_h */
