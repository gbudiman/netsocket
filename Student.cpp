//
//  Student.cpp
//  netsocket
//
//  Created by Gloria Budiman on 7/16/16.
//  Copyright © 2016 gbudiman. All rights reserved.
//

#include <stdio.h>
#include "main.h"
#include "Student.h"
#include "StudentParser.hpp"

int main() {
  spawn_iterative(NUM_STUDENTS);
  //spawn_one();

  return 0;
}

void spawn_one() {
  do_work(1);
}

void spawn_iterative(int n) {
  for (int i = 0; i < n; i++) {
    switch(fork()) {
      case 0:
        do_work(i + 1);
        exit(0);
        break;
      case -1:
        std::cerr << "Fork error at iteration " << i << "\n";
        break;
      default:
        wait(NULL);
        break;;
    }
  }
  
  wait(NULL);
}

void do_work(uint32_t id) {
  if (PROJ_DEBUG) {
    std::cout << "Process " << ::getpid() << " created for student " << id << "\n";
  }
  
  
  StudentParser *sp = new StudentParser(id);
  connect_to_admission_server(sp, id);
  
  if (PROJ_DEBUG) {
    std::cout << "Process " << ::getpid() << " returned\n";
  }
  
  exit(0);
}

int connect_to_admission_server(StudentParser *sp, uint32_t student_id) {
  int sockfd = Socket::create_socket(TCP_CLIENT);
  send_data_to_admission_server(student_id, sockfd, sp);
  close(sockfd);
  return 0;
}

int send_data_to_admission_server(int student_id, int sockfd, StudentParser *sp) {
  char s_msg[MAXDATASIZE];
  char adm_resp[MAXDATASIZE];
  
  sprintf(s_msg, "I_AM_STUDENT#%d", student_id);
  send(sockfd, s_msg, strlen(s_msg), 0);
  
  while(1) {
    recv(sockfd, adm_resp, sizeof(adm_resp), 0);
    
    if (PROJ_DEBUG) {
      std::cout << "Received " << adm_resp << "\n";
    }
    
    if (strcmp(((std::string) adm_resp).substr(0, 10).c_str(), "ADM_I_RCGZ") == 0) {
      if (PROJ_DEBUG) {
        std::cout << "ADM_I_RCGZ received\n";
      }
      break;
    }
  }
  
  if (PROJ_DEBUG) {
    std::cout << "Admission has recognized student. Now sending GPA data...\n";
  }
  
  sprintf(s_msg, "%d#GPA#%.1f", student_id, sp->gpa);
  send(sockfd, s_msg, strlen(s_msg), 0);
  while(1) {
    recv(sockfd, adm_resp, sizeof(adm_resp), 0);
    
    if (strcmp(((std::string) adm_resp).substr(0, 9).c_str(), "ADM_RX_OK") == 0) {
      break;
    }
  }
  
  for (std::vector<std::string>::iterator s = sp->interests->begin(); s != sp->interests->end(); ++s) {
    sprintf(s_msg, "%d#%s", student_id, s->c_str());
    send(sockfd, s_msg, strlen(s_msg), 0);
    
    while(1) {
      recv(sockfd, adm_resp, sizeof(adm_resp), 0);
      
      if (strcmp(((std::string) adm_resp).substr(0, 9).c_str(), "ADM_RX_OK") == 0) {
        break;
      }
    }
  }
  
  sprintf(s_msg, "TX_FIN");
  send(sockfd, s_msg, strlen(s_msg), 0);
  
  
  return 0;
}
