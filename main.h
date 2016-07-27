//
//  main.h
//  netsocket
//
//  Created by Gloria Budiman on 7/16/16.
//  Copyright © 2016 gbudiman. All rights reserved.
//

#ifndef main_h
#define main_h

#define PROJ_DEBUG 0

#define NUM_STUDENTS 5
#define NUM_DEPTS 3

#define DEPT_WAIT_LIMIT_SECOND 20     // second
#define DEPT_SLEEP_BETWEEN_PROGRAM 1  // second

#define ADMISSION_PORT "3950"
#define ADMISSION_BACKLOG 24

#define DEPARTMENT_BASE_UDP_PORT 21750
#define STUDENT_BASE_UDP_PORT 22050

#define DATABASE_FILE "database.txt"

#define MAXDATASIZE 255

#define DISABLE_PHASE_1 1
#define ENABLE_PHASE_2 1

#define SERVER "localhost"

#define TCP_CLIENT 1
#define TCP_SERVER 0

#define UDP_LISTENER 1
#define UDP_TALKER 0

#define PROCEED_WITH_ITERATION 1
#define NO_MORE_ITERATION 0

#include <arpa/inet.h>
#include <errno.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <vector>
#include <map>

#endif /* main_h */
