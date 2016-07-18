//
//  Admission.cpp
//  netsocket
//
//  Created by Gloria Budiman on 7/16/16.
//  Copyright © 2016 gbudiman. All rights reserved.
//

#include <stdio.h>
#include "main.h"
#include "Admission.h"

void sigchld_handler(int s) {
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*) sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

int main() {
  srand(time(NULL));
  std::map<std::string, float> *database = new std::map<std::string, float>();
  
  int sockfd, new_fd;
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  struct sigaction sa;
  int yes = 1;
  char s[INET6_ADDRSTRLEN];
  char receive_buffer[MAXDATASIZE];
  int receive_length;
  int rv;
  
  memset(&hints, 0 ,sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  
  if ((rv = getaddrinfo(NULL, ADMISSION_PORT, &hints, &servinfo)) != 0) {
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
  
  fm_self_tcp_ip(p, ADMISSION_PORT);
  
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
      perror("accept");
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
      if (send(new_fd, "Hello, child!", 13, 0) == -1) {
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
            
            fm_dept_completed(current_dept);
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
    }
    
    close(new_fd);
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

void fm_self_tcp_ip(addrinfo *p, char *port) {
  struct sockaddr_storage my_addr;
  char host_ip[255];
  std::vector<std::string> *args = new std::vector<std::string>();
  
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) &p), host_ip, sizeof(host_ip));
  args->push_back(host_ip);
  args->push_back(port);
  
  flow_message(AMSG_P1_START, args);
}

void fm_dept_completed(char dept_name) {
  std::vector<std::string> *args = new std::vector<std::string>();
  std::string dept_name_s = "";
  dept_name_s = dept_name;
  
  args->push_back(dept_name_s);
  flow_message(AMSG_DEPT_COMPLETED, args);
}

void flow_message(int type, std::vector<std::string>* args) {
  switch(type) {
    case AMSG_P1_START:
      std::cout << "The admission office has TCP port " << args->at(0)
      << " and IP " << args->at(1) << "\n";
      break;
    case AMSG_DEPT_COMPLETED:
      std::cout << "Received the program list from " << args->at(0) << "\n";
      break;
    case AMSG_P1_END:
      std::cout << "End of Phase 1 for the admission office\n";
  }
}
