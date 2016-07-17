//
//  Department.h
//  netsocket
//
//  Created by Gloria Budiman on 7/16/16.
//  Copyright © 2016 gbudiman. All rights reserved.
//

#ifndef Department_h
#define Department_h
#include "DepartmentParser.hpp"

int do_work(char);
int connect_to_admission_server(DepartmentParser*);
int send_data_to_admission_server(int, DepartmentParser*);
void spawn(int);

#endif /* Department_h */
