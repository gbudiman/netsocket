//
//  Department.cpp
//  netsocket
//
//  Created by Gloria Budiman on 7/16/16.
//  Copyright © 2016 gbudiman. All rights reserved.
//

#include <stdio.h>
#include "main.h"
#include "Department.h"
#include "DepartmentParser.hpp"

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*) sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

int main() {
  int32_t pid[NUM_DEPTS];
  
  for (uint32_t p = 0; p < NUM_DEPTS; p++) {
    pid[p] = fork();
    
    if (pid[p] < 0) {
      std::cerr << "Forking failed at iteration " << p << "\n";
    } else if (pid[p] == 0) {
      do_work(p + 0x41);
    }
  }
  
  return 0;
}

void do_work(char d) {
  if (PROJ_DEBUG) {
    std::cout << "Process " << ::getpid() << " created for department " << d << "\n";
  }
  
  std::string dept_name = "";
  dept_name += d;
  new DepartmentParser(dept_name);
  
  int connection_status = connect_to_admission_server();
  
  if (PROJ_DEBUG) {
    std::cout << "Process " << ::getpid() << " returned\n";
  }
  
  exit(connection_status);
}

int connect_to_admission_server() {
  int sockfd, numbytes;
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  
  if ((rv = getaddrinfo("localhost", ADMISSION_PORT, &hints, &servinfo)) != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(rv) << "\n";
    return 1;
  }
  
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }
    
    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("client: connect");
      continue;
    }
    
    break;
  }
  
  if (p == NULL) {
    std::cerr << "Department Client failed to connect\n";
    return 2;
  }
  
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr), s, sizeof(s));
  std::cout << "Department Client connecting to " << s << "\n";
  freeaddrinfo(servinfo);
  
  if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
    perror("recv");
    exit(1);
  }
  
  buf[numbytes] = '\0';
  std::cout << "Department Client received: " << buf << "\n";
  send_data_to_admission_server(sockfd);

  close(sockfd);
  return 0;
}

int send_data_to_admission_server(int sockfd) {
  std::string msg = "Hello Admission Server";
  int len = (int) strlen(msg.c_str());
  int bytes_sent = send(sockfd, msg.c_str(), len, 0);
  
  std::cout << bytes_sent << " bytes sent\n";
  return 0;
}
