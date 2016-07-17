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

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*) sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

int main() {
  spawn(NUM_DEPTS);
  
  return 0;
}

void spawn(int n) {
  if (n > 0) {
    if (fork() == 0) {
      do_work(n + 0x40);
      if (n) {
        spawn(n - 1);
      } else {
        wait(NULL);
        return;
      }
    }
  }
}

int do_work(char d) {
  if (PROJ_DEBUG) {
    std::cout << "Process " << ::getpid() << " created for department " << d << "\n";
  }
  
  std::string dept_name = "";
  dept_name += d;
  DepartmentParser *dp = new DepartmentParser(dept_name);
  
  int connection_status = connect_to_admission_server(dp);
  
  if (PROJ_DEBUG) {
    std::cout << "Process " << ::getpid() << " returned\n";
  }
  
  return connection_status;
}

int connect_to_admission_server(DepartmentParser *dp) {
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
  send_data_to_admission_server(sockfd, dp);

  close(sockfd);
  return 0;
}

int send_data_to_admission_server(int sockfd, DepartmentParser *dp) {
//  std::string msg = "Hello Admission Server";
//  int len = (int) strlen(msg.c_str());
//  int bytes_sent = send(sockfd, msg.c_str(), len, 0);
//  
//  std::cout << bytes_sent << " bytes sent\n";
  int len;
  int bytes_sent;
  int count = 1;
  char d_msg[MAXDATASIZE];
  
  for (std::map<std::string, float>::iterator r = dp->requirements->begin(); r != dp->requirements->end(); ++r) {
    
    
    sprintf(d_msg, "%s#%.1f", r->first.c_str(), r->second);
    // std::cout << "Sending message " << d_msg << "\n";
    len = (int) strlen(d_msg);
    bytes_sent = send(sockfd, d_msg, len, 0);
    std::cout << bytes_sent << " bytes sent\n";
    sleep(DEPT_SLEEP_BETWEEN_PROGRAM);
    std::cout << "sleeping...\n";
    
    count++;
  }
  
  sprintf(d_msg, "TX_FIN");
  bytes_sent = send(sockfd, d_msg, len, 0);
  std::cout << bytes_sent << " bytes sent\n";
  
  std::cout << "Done transmitting!\n";
  return 0;
}
