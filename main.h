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

#define ADMISSION_PORT "5553"
#define ADMISSION_BACKLOG 24

#define MAXDATASIZE 255

#define SERVER "localhost"

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
#include <sstream>
#include <vector>
#include <map>

#endif /* main_h */
