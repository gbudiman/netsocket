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
  // spawn_recursive(NUM_DEPTS);
  spawn_iterative(NUM_DEPTS);
  wait(NULL);
  
  return 0;
}

void spawn_iterative(int n) {
  pid_t pid;
  for (int i = 0; i < n; i++) {
//    pid = fork();
//    if (pid) {
//      wait(NULL);
//    } else if (pid == 0) {
//      do_work(i + 0x41);
//      exit(0);
//    } else {
//      std::cerr << "For error at iteration " << i << "\n";
//    }
    
    switch(fork()) {
      case 0:
        do_work(i + 0x41);
        exit(0);
        break;
      case -1:
        std::cerr << "Fork error at iteration " << i << "\n";
        break;
      default: ;
    }
  }
}

void spawn_recursive(int n) {
  if (n > 0) {
    if (fork() == 0) {
      do_work(n + 0x40);
      if (n) {
        spawn_recursive(n - 1);
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
  
  int connection_status = connect_to_admission_server(dp, d);
  
  if (PROJ_DEBUG) {
    std::cout << "Process " << ::getpid() << " returned\n";
  }
  
  return connection_status;
}

int connect_to_admission_server(DepartmentParser *dp, char dept_name) {
  int sockfd = 0;
  int numbytes = 0;
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage my_addr;
  socklen_t my_addr_len = sizeof(my_addr);
  int rv;
  int peer_port;
  char peer_ipstr[INET6_ADDRSTRLEN];
  char host_name[MAXDATASIZE];
  char host_ipstr[INET6_ADDRSTRLEN];
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  
  if ((rv = getaddrinfo("localhost", ADMISSION_PORT, &hints, &servinfo)) != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(rv) << "\n";
    return 1;
  }
  
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      //perror("client: socket");
      continue;
    }
    
    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      //perror("client: connect");
      continue;
    }
    
    break;
  }
  
  if (p == NULL) {
    std::cerr << "Department Client failed to connect\n";
    return 2;
  }

  getpeername(sockfd, (struct sockaddr *) &my_addr, &my_addr_len);
  if (my_addr.ss_family == AF_INET) {
    struct sockaddr_in *sockfd = (struct sockaddr_in *) &my_addr;
    peer_port = ntohs(sockfd->sin_port);
    inet_ntop(AF_INET, &sockfd->sin_addr, peer_ipstr, sizeof(peer_ipstr));
  }
  
  //gethostname(host_ipstr, sizeof(host_ipstr));
  
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr), host_ipstr, sizeof(host_ipstr));
  std::cout << "Department " << dept_name
            << " has TCP port " << peer_port
            << " and IP " << host_ipstr << "\n";
  //std::cout << "Department Client connecting to " << s << "\n";
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
  int len = 0;
  int bytes_sent = 0;
  int count = 1;
  char d_msg[MAXDATASIZE];
  char adm_resp[MAXDATASIZE];
  
  for (std::map<std::string, float>::iterator r = dp->requirements->begin(); r != dp->requirements->end(); ++r) {
    
    
    sprintf(d_msg, "%s#%.1f", r->first.c_str(), r->second);
    len = (int) strlen(d_msg);
    bytes_sent = (int) send(sockfd, d_msg, len, 0);
    std::cout << bytes_sent << " bytes sent: " << d_msg << "\n";
    //sleep(DEPT_SLEEP_BETWEEN_PROGRAM);
    //std::cout << "sleeping...\n";
    
    std::cout << "Waiting for acknowledgement from Admission Server...\n";
    while(1) {
      recv(sockfd, adm_resp, sizeof(adm_resp), 0);
      
      std::cout << "Received " << adm_resp << "\n";
      if (strcmp(adm_resp, "ADM_RX_OK") == 0) {
        break;
      }
    }
    
    count++;
  }
  
  sprintf(d_msg, "TX_FIN");
  bytes_sent = send(sockfd, d_msg, len, 0);
  std::cout << bytes_sent << " bytes sent: " << d_msg << "\n";
  
  std::cout << "Done transmitting!\n";
  return 0;
}
