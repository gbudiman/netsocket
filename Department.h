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
#include "Socket.hpp"

void initialize_children_trackers();
void wait_for_children();
int do_work(char);
int connect_to_admission_server(DepartmentParser*, char);
int send_data_to_admission_server(char, int, DepartmentParser*);
void spawn_recursive(int);
void spawn_iterative(int);
void spawn_one();

std::string get_socket_port(int);

DepartmentMessenger *dm = new DepartmentMessenger();
std::vector<int> *child_processes;

#endif /* Department_h */
