//
//  Admission.cpp
//  netsocket
//
//  Created by Gloria Budiman on 7/16/16.
//  Copyright © 2016 gbudiman. All rights reserved.
//

#include <stdio.h>
#include "Admission.h"

void sigchld_handler(int s) {
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
  srand(time(NULL));
  std::map<std::string, float> *database = new std::map<std::string, float>();
  
  int sockfd = 0, new_fd;
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  
  int depts_completed = 0;
  int yes = 1;
  char s[INET6_ADDRSTRLEN];
  char receive_buffer[MAXDATASIZE];
  int receive_length;
  int rv;
  
  struct sigaction sa;
  
  memset(&hints, 0 ,sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  
  if ((rv = getaddrinfo(SERVER, ADMISSION_PORT, &hints, &servinfo)) != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(rv);
    return 1;
  }
  
  for(p = servinfo; p != NULL; p = p->ai_next) {
/////////////////////////////
// SOCKET()
/////////////////////////////
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

/////////////////////////////
// BIND()
/////////////////////////////
    
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }
    
    break;
  }
  
  if (p == NULL) {
    std::cerr << "Admission Server: failed to bind\n";
    return 2;
  }
  
  am->display_tcp_ip(get_socket_port(sockfd), get_self_ip_address());

  freeaddrinfo(servinfo);
  
/////////////////////////////
// LISTEN()
/////////////////////////////
  if (listen(sockfd, ADMISSION_BACKLOG) == -1) {
    perror("listen");
    exit(-1);
  }
  
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }
  
  if (PROJ_DEBUG) {
    std::cout << "Admission Server: waiting for connections...\n";
  }
  
  while(1) {
    sin_size = sizeof(their_addr);
/////////////////////////////
// ACCEPT()
/////////////////////////////
    new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
    if (new_fd == -1) {
      //perror("accept");
      continue;
    }
    
    
    if (PROJ_DEBUG) {
      std::cout << "Admission Server: got connection from " << s << "\n";
    }

    if (!fork()) {
      close(sockfd);
/////////////////////////////
// SEND()
/////////////////////////////
      
      std::string client_ip = get_client_ip_address(new_fd);
      if (send(new_fd, client_ip.c_str(), client_ip.length(), 0) == -1) {
        perror("send");
      }
      
      while (1) {
/////////////////////////////
// RECV()
/////////////////////////////
        int receive_length = (int) recv(new_fd, receive_buffer, MAXDATASIZE - 1, 0);
        char current_dept;
        
        if (receive_length > 0) {
          std::string r_msg = debug_receive_buffer(receive_buffer, receive_length);
        
          if (strcmp(r_msg.c_str(), "TX_FIN") == 0) {
            if (PROJ_DEBUG) {
              std::cout << "TX_FIN signal received. Closing socket " << new_fd << "\n";
            }
            
            //fm_dept_completed(current_dept);
            am->display_department_completed(current_dept);
            break;
          } else {
            
            int rand_wait = rand() % 500 + 500;
            usleep(rand() % 500 + 500);
            int pdm_error;
            pdm_error = process_department_message(receive_buffer, receive_length, database);
            
            if (pdm_error == 0) {
              current_dept = receive_buffer[0];
              if (send(new_fd, "ADM_RX_OK", 9, 0) == -1) {
                perror("dept ack");
              } else {
                if (PROJ_DEBUG) {
                  std::cout << "ADM_RX_OK sent out after randomly waiting for " << rand_wait << " us\n";
                }
              }
            }
          }
        }
      }
      
      close(new_fd);
      exit(0);
    } else {
      wait(NULL);
      depts_completed++;
    }
    
    close(new_fd);
    check_department_completion(&depts_completed);
  }
  
  return 0;
}

uint32_t process_department_message(char *buffer, int length, std::map<std::string, float> *db) {
  char *element;
  uint32_t e_pos = 0;
  std::string dept_program = "";
  float dept_program_min_gpa = 0;
  
  element = strtok(buffer, "#");
  while (element != NULL) {
    switch(e_pos) {
      case 0:
        dept_program = element;
        break;
      case 1:
        dept_program_min_gpa = std::atof(element);
        break;
    }
    element = strtok(NULL, "#");
    e_pos++;
  }
  
  db->insert(std::make_pair(dept_program, dept_program_min_gpa));
  return 0;
}

void check_department_completion(int *size) {
  if (*size == NUM_DEPTS) {
    //fm_phase1_completed();
    am->display_phase1_completed();
    *size = 0;
  }
}

std::string debug_receive_buffer(char *receive_buffer, int receive_length) {
  std::string result = "";
  
  if (PROJ_DEBUG) {
    std::cout << "Received message (" << receive_length << " bytes): ";
  }
  
  for (int i = 0; i < receive_length; i++) {
    if (PROJ_DEBUG) {
      printf("%c", receive_buffer[i]);
    }
    result += receive_buffer[i];
  }
  
  if (PROJ_DEBUG) {
    std::cout << "\n";
  }
  
  return result;
}

std::string get_client_ip_address(int sockfd) {
  struct sockaddr_storage addr;
  char ipstr[INET6_ADDRSTRLEN];
  socklen_t addrlen = sizeof(addr);
  
  getpeername(sockfd, (struct sockaddr *) &addr, &addrlen);
  struct sockaddr_in *s = (struct sockaddr_in *) &addr;
  
  inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof(ipstr));
  return (std::string) ipstr;
}

std::string get_socket_port(int sockfd) {
  struct sockaddr_in sin;
  socklen_t addrlen = sizeof(sin);
  getsockname(sockfd, (struct sockaddr *) &sin, &addrlen);
  
  int local_port = ntohs(sin.sin_port);
  char local_port_s[MAXDATASIZE];
  sprintf(local_port_s, "%d", local_port);
  return (std::string) local_port_s;
}

std::string get_self_ip_address() {
  struct addrinfo hints, *res, *p;
  char ipstr[INET6_ADDRSTRLEN];
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  
  getaddrinfo(SERVER, NULL, &hints, &res);
  
  for (p = res; p != NULL; p = p->ai_next) {
    void *addr;
    struct sockaddr_in *ip = (struct sockaddr_in *) p->ai_addr;
    addr = &(ip->sin_addr);
    
    inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
  }
  
  freeaddrinfo(res);
  return (std::string) ipstr;
}
