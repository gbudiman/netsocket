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

int main() {
  spawn_iterative(NUM_DEPTS);
  //spawn_one();
  // wait(NULL);
  
  return 0;
}

void spawn_one() {
  do_work(0x41);
}

void spawn_iterative(int n) {
  for (int i = 0; i < n; i++) {
    switch(fork()) {
      case 0:
        do_work(i + 0x41);
        exit(0);
        break;
      case -1:
        std::cerr << "Fork error at iteration " << i << "\n";
        break;
      default:
        break;;
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
  std::string dept_name = ""; dept_name += d;
  DepartmentParser *dp = new DepartmentParser(dept_name);
  dm = new DepartmentMessenger();
  dm->set_department_name(dept_name);
  
  if (PROJ_DEBUG) {
    std::cout << "Process " << ::getpid() << " created for department " << d << "\n";
  }
  
  //get_self_interfaces_info();
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
  int rv;
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  
  if ((rv = getaddrinfo(SERVER, ADMISSION_PORT, &hints, &servinfo)) != 0) {
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

  //fm_self_tcp_ip(dept_name, p, (char*) ADMISSION_PORT);
  dm->display_tcp_ip(p, (char*) ADMISSION_PORT);
  freeaddrinfo(servinfo);
  
  if ((numbytes = (int) recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
    perror("recv");
    exit(1);
  }
  
  buf[numbytes] = '\0';
  //fm_self_connected(dept_name);
  dm->display_connected();
  send_data_to_admission_server(dept_name, sockfd, dp);
  dm->display_phase1_completed();
  //fm_self_phase1_done(dept_name);

  close(sockfd);
  return 0;
}

int send_data_to_admission_server(char dept_name, int sockfd, DepartmentParser *dp) {
  int len = 0;
  int bytes_sent = 0;
  int count = 1;
  char d_msg[MAXDATASIZE];
  char adm_resp[MAXDATASIZE];
  
  for (std::map<std::string, float>::iterator r = dp->requirements->begin(); r != dp->requirements->end(); ++r) {
    
    sprintf(d_msg, "%s#%.1f", r->first.c_str(), r->second);
    len = (int) strlen(d_msg);
    bytes_sent = (int) send(sockfd, d_msg, len, 0);
    
    //fm_self_program_sent(dept_name, r->first);
    dm->display_one_program_sent(r->first);
    
    if (PROJ_DEBUG) {
      std::cout << bytes_sent << " bytes sent: " << d_msg << "\n";
      std::cout << "Waiting for acknowledgement from Admission Server...\n";
    }
    
    while(1) {
      recv(sockfd, adm_resp, sizeof(adm_resp), 0);
      
      if (PROJ_DEBUG) {
        std::cout << "Received " << adm_resp << "\n";
      }
      if (strcmp(((std::string) adm_resp).substr(0, 9).c_str(), "ADM_RX_OK") == 0) {
        if (PROJ_DEBUG) {
          std::cout << "ADM_RX_OK received\n";
        }
        break;
      }
    }
    
    count++;
  }
  
  sprintf(d_msg, "TX_FIN");
  bytes_sent = send(sockfd, d_msg, len, 0);
  
  if (PROJ_DEBUG) {
    std::cout << bytes_sent << " bytes sent: " << d_msg << "\n";
    std::cout << "Done transmitting!\n";
  }
  
  // fm_self_all_program_sent(dept_name);
  dm->display_all_program_sent();
  
  return 0;
}

//void get_self_interfaces_info() {
//  struct ifaddrs *ifaddr, *ifa;
//  int family, s, n;
//  char host[NI_MAXHOST];
//  
//  if (getifaddrs(&ifaddr) == -1) {
//    perror("getifaddrs");
//  }
//  
//  for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
//    if (ifa->ifa_addr == NULL) {
//      continue;
//    }
//    
//    family = ifa->ifa_addr->sa_family;
//    
//    std::cout << ifa->ifa_name
//              << ((family == AF_INET) ? "AF_INET" :
//                  (family == AF_INET6) ? "AF_INET6" : "???");
//    
//    if (family == AF_INET || family == AF_INET6) {
//      s = getnameinfo(ifa->ifa_addr,
//                      (family == AF_INET) ? sizeof(struct sockaddr_in) :
//                                            sizeof(struct sockaddr_in6),
//                      host, NI_MAXHOST,
//                      NULL, 0, NI_NUMERICHOST);
//      
//      if (s != 0) {
//        std::cout << "getnameinfo() failed: " << gai_strerror(s);
//      }
//      
//      std::cout << "Address: " << host << "\n";
//    }
//  }
//  
//  freeifaddrs(ifaddr);
//}
