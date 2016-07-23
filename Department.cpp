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
  
  // wait_for_children();
  
  if (ENABLE_PHASE_2) {
    //sleep(10);
    //std::cout << "begin phase 2\n";
  }
  return 0;
}

//void wait_for_children() {
//  if (PROJ_DEBUG) {
//    std::cout << "Spin-waiting for children to complete\n";
//  }
//  
//  int current_child_pos = 0;
//  while (current_child_pos < NUM_DEPTS) {
//    if (child_processes->at(current_child_pos) == 0) {
//      current_child_pos++;
//    }
//  }
//  
//  if (PROJ_DEBUG) {
//    std::cout << "Spin-wait completed, current_child_pos is " << current_child_pos << "\n";
//  }
//}

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
        wait(NULL);
        break;;
    }
  }
  
  wait(NULL);
  //exit(0);
}

//void spawn_recursive(int n) {
//  if (n > 0) {
//    if (fork() == 0) {
//      do_work(n + 0x40);
//      if (n) {
//        spawn_recursive(n - 1);
//      } else {
//        wait(NULL);
//        return;
//      }
//    }
//  }
//}

int do_work(char d) {
  std::string dept_name = ""; dept_name += d;
  DepartmentParser *dp = new DepartmentParser(dept_name);
  dm = new DepartmentMessenger();
  dm->set_department_name(dept_name);
  
  if (PROJ_DEBUG) {
    std::cout << "Process " << ::getpid() << " created for department " << d << "\n";
  }
  
  int connection_status = connect_to_admission_server(dp, d);
  
  if (PROJ_DEBUG) {
    std::cout << "Process " << ::getpid() << " returned\n";
  }
  
  return connection_status;
}

int connect_to_admission_server(DepartmentParser *dp, char dept_name) {
  int sockfd = Socket::create_socket(TCP_CLIENT);
  dm->display_tcp_ip(Socket::get_socket_port(sockfd), Socket::get_self_ip_address());
  dm->display_connected();
  send_data_to_admission_server(dept_name, sockfd, dp);
  dm->display_phase1_completed();

  close(sockfd);
  return 0;
}

int send_data_to_admission_server(char dept_name, int sockfd, DepartmentParser *dp) {
  int len = 0;
  int bytes_sent = 0;
  char d_msg[MAXDATASIZE];
  char adm_resp[MAXDATASIZE];
  
  sprintf(d_msg, "I_AM_DEPARTMENT#%c", dept_name);
  len = (int) strlen(d_msg);
  send(sockfd, d_msg, len, 0);
  
  while(1) {
    recv(sockfd, adm_resp, sizeof(adm_resp), 0);
    
    if (PROJ_DEBUG) {
      std::cout << "Received " << adm_resp << "\n";
    }
    
    if (strcmp(((std::string) adm_resp).substr(0, 10).c_str(), "ADM_I_RCGZ") == 0) {
      if (PROJ_DEBUG) {
        std::cout << "ADM_I_RCGZ received \n";
      }
      break;
    }
  }
  
  if (PROJ_DEBUG) {
    std::cout << "Admission has recognized department. Now sending data...\n";
  }
  
  for (std::map<std::string, float>::iterator r = dp->requirements->begin(); r != dp->requirements->end(); ++r) {
    
    sprintf(d_msg, "%s#%.1f", r->first.c_str(), r->second);
    len = (int) strlen(d_msg);
    bytes_sent = (int) send(sockfd, d_msg, len, 0);
    
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
