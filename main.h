//
//  main.h
//  netsocket
//
//  Created by Gloria Budiman on 7/16/16.
//  Copyright © 2016 gbudiman. All rights reserved.
//

#ifndef main_h
#define main_h

#if !defined PROJ_DEBUG
#define PROJ_DEBUG 1
#endif

#define NUM_STUDENTS 5
#define NUM_DEPTS 3

#define ADMISSION_PORT "19230"
#define ADMISSION_BACKLOG 24

#define MAXDATASIZE 255

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#endif /* main_h */
