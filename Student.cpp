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
        //wait(NULL);
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
  sm = new StudentMessenger();
  sm->set_student_name(id);
  connect_to_admission_server(sp, id);
  wait_for_admission_response(id);
  
  if (PROJ_DEBUG) {
    std::cout << "Process " << ::getpid() << " returned\n";
  }
  
  exit(0);
}

void wait_for_admission_response(uint32_t student_id) {
  char port_s[MAXDATASIZE] = "";
  int student_port = STUDENT_BASE_UDP_PORT + 100 * (student_id - 1);
  sprintf(port_s, "%d", student_port);
  char buffer[MAXDATASIZE] = "";
  int numbytes = 0;
  struct sockaddr_storage their_addr;
  socklen_t addr_len;
  
  //int sockfd = Socket::create_udp_socket(UDP_LISTENER, port_s, p);
  
  int rv;
  int sockfd = 0;
  struct addrinfo hints, *servinfo, *p;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;
  
  if ((rv = getaddrinfo(SERVER, port_s, &hints, &servinfo)) != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(rv);
  }
  
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("listener: socket");
      continue;
    }
    
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      continue;
    }
    
    break;
  }
  
  if (p == NULL) {
    perror("listener");
  }
  
  freeaddrinfo(servinfo);
  
  memset(&hints, 0, sizeof(hints));
  
  if (PROJ_DEBUG) {
    std::cout << "Student " << student_id <<  " is waiting for Admission response on port " << port_s << "...\n";
  }
  
  while(true) {
    if ((numbytes = recvfrom(sockfd, buffer, MAXDATASIZE - 1, 0, (struct sockaddr *) &their_addr, &addr_len)) == -1) {
      std::cout << "Student " << student_id << " encountered recvfrom error\n";
      perror("recvfrom");
      break;
    }
    
    if (numbytes > 0) {
      buffer[numbytes] = '\0';
      if (PROJ_DEBUG) {
        std::cout << "Student " << student_id << " received response " << buffer << "\n";
      }
      
      if (strcmp(buffer, "ADM_END") == 0) {
        if (PROJ_DEBUG) {
          std::cout << "ADM_END received. Terminating process\n";
        }
        
        break;
      }
    }
  }
  
  close(sockfd);
}

int connect_to_admission_server(StudentParser *sp, uint32_t student_id) {
  int sockfd = Socket::create_socket(TCP_CLIENT);
  sm->display_tcp_ip(Socket::get_socket_port(sockfd), Socket::get_self_ip_address());
  send_data_to_admission_server(student_id, sockfd, sp);
  sm->display_all_applications_sent();
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
