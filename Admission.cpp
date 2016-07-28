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
  if (!DISABLE_PHASE_1) {
    create_empty_database_file();
    create_tcp_and_process();
  }
  
  if (ENABLE_PHASE_2) {
    make_admission_decision();
    create_udp_and_process();
  }
  
  return 0;
}

void create_empty_database_file() {
  std::ofstream fs;
  fs.open(DATABASE_FILE, std::ios::out);
  fs.close();
  db = new Database();
}

void make_admission_decision() {
  db->build();
  db->make_decision();
  
  if (PROJ_DEBUG) {
    db->debug_decision();
  }
}

void create_udp_and_process() {
  std::ofstream f;
  f.open(DATABASE_FILE, std::ios::app);
  
  //for (int s = 0; s < NUM_STUDENTS; s++) {
  for (std::vector<std::string>::iterator d = db->decision->begin(); d != db->decision->end(); ++d) {
    
    int student_id = 0;
    int token_position = 0;
    char *element;
    int sockfd = 0;
    int numbytes = 0;
    int rv;
    struct addrinfo hints, *servinfo = NULL, *p = NULL;
    char port_s[MAXDATASIZE] = "";
    char *decision_s = new char[d->size() + 1];
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    
    std::copy(d->begin(), d->end(), decision_s);
    decision_s[d->size()] = '\0';
    element = std::strtok((char*) d->c_str(), "#");
    while (element != NULL) {
    
      if (token_position == 0) {
        student_id = atoi(element);
        
        int student_port = STUDENT_BASE_UDP_PORT + 100 * (student_id - 1);
        sprintf(port_s, "%d", student_port);
        
        if ((rv = getaddrinfo(SERVER, port_s, &hints, &servinfo)) != 0) {
          std::cerr << "getaddrinfo talker: " << gai_strerror(rv);
        }
        
        for (p = servinfo; p != NULL; p = p->ai_next) {
          if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
          }
          
          break;
        }
        
        if (p == NULL) {
          std::cerr << "talker -> student: failed to bind socket\n";
        }
      } else if (token_position == 1) {
        numbytes = sendto(sockfd, &(decision_s[2]), strlen(decision_s) - 2, 0, p->ai_addr, p->ai_addrlen);
        
        if (PROJ_DEBUG) {
          std::cout << "Message to student " << student_id << ": " << decision_s << "\n";
          std::cout << "Sent " << numbytes << " bytes to port " << port_s << "\n";
        }
        
        numbytes = sendto(sockfd, "ADM_END", 7, 0, p->ai_addr, p->ai_addrlen);
        
        am->display_udp_ip(Socket::get_socket_port(sockfd), Socket::get_self_ip_address());
        am->display_student_admission_result((char) student_id + 0x30);
        if (PROJ_DEBUG) {
          std::cout << "End of transmission to student " << student_id << "\n";
        }
        
        f << &(decision_s[2]);
        f << "\n";
        
        freeaddrinfo(servinfo);
        close(sockfd);
      } else if (token_position == 2) {
        // Send admission decision to respective Department
        servinfo = NULL;
        p = NULL;
        std::string message = "";
        char *message_s;
        char student_id_s = (char) student_id + 0x30;
        char gpa_s[MAXDATASIZE] = "";
        char department_id = element[0];
        int department_port = DEPARTMENT_BASE_UDP_PORT + 100 * (department_id - 0x41);
        sprintf(port_s, "%d", department_port);
        sprintf(gpa_s, "%.1f", db->student_grades->at(student_id));
        
        message = "Student";
        message += student_id_s;
        message += "#";
        message += gpa_s;
        message += "#";
        message += element;
        message_s = new char[message.size() + 1];
        std::copy(message.begin(), message.end(), message_s);
        message_s[message.size()] = '\0';
        
        if ((rv = getaddrinfo(SERVER, port_s, &hints, &servinfo)) != 0) {
          std::cerr << "getaddrinfo talker -> department: " << gai_strerror(rv);
        }
        
        for (p = servinfo; p != NULL; p = p->ai_next) {
          if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
          }
          break;
        }
      
        if (p == NULL) {
          std::cerr << "talker -> department: failed to bind socket\n";
        }
        
        numbytes = sendto(sockfd, message_s, strlen(message_s), 0, p->ai_addr, p->ai_addrlen);
        
        am->display_udp_ip(Socket::get_socket_port(sockfd), Socket::get_self_ip_address());
        am->display_department_admission_result(department_id);
        if (numbytes == -1) {
          perror("talker: sendto -> department");
        }
        
        if (PROJ_DEBUG) {
          std::cout << "Attempt to send message of size " << strlen(message_s);
          std::cout << "Message to Department " << department_id << " at port " << port_s << "\n";
          std::cout << numbytes << " bytes: " << message_s << "\n";
        }
        
        f << message_s << "\n";
        close(sockfd);
      }
      
      
      element = strtok(NULL, "#");
      token_position++;
    }
    
    delete[] decision_s;
  }
  
  // All departments have been notified of admission
  // Tell each departments to close all their UDP sockets
  for (int dept_id = 0; dept_id < NUM_DEPTS; dept_id++) {
    char port_s[MAXDATASIZE] = "";
    int sockfd = 0;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes = 0;
    
    sprintf(port_s, "%d", DEPARTMENT_BASE_UDP_PORT + 100 * (dept_id));
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    
    getaddrinfo(SERVER, port_s, &hints, &servinfo);
    for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
        perror("talker -> department -> closure: socket");
        continue;
      }
      
      break;
    }
    
    if (p == NULL) {
      perror("talker -> department -> closure: failed to bind socket");
    }
    
    numbytes = sendto(sockfd, "ADM_END", 7, 0, p->ai_addr, p->ai_addrlen);
    if (PROJ_DEBUG) {
      std::cout << "Sent " << numbytes << " bytes to close UDP socket to port " << port_s << "\n";
    }
    freeaddrinfo(servinfo);
    close(sockfd);
  }
  
  am->display_phase2_completed();
  f.close();
}

void create_tcp_and_process() {
  srand(time(NULL));
  
  int sockfd = 0, new_fd;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  
  char s[INET6_ADDRSTRLEN];
  int tcp_client_type = 0;
  
  struct sigaction sa;
  
  sockfd = Socket::create_socket(TCP_SERVER);
  am->display_tcp_ip(Socket::get_socket_port(sockfd), Socket::get_self_ip_address());
  
  
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
      int matching_program = 0;
      
      while (1) {
        int receive_length = (int) recv(new_fd, receive_buffer, MAXDATASIZE - 1, 0);
        char current_dept;
        char current_student;
        
        if (receive_length > 0) {
          std::string r_msg = debug_receive_buffer(receive_buffer, receive_length);
          
          if (tcp_client_type == 0) {
            if (PROJ_DEBUG) {
              std::cout << "Received introduction: " << r_msg << "\n";
            }
            
            if (strcmp(r_msg.substr(0, 12).c_str(), "I_AM_STUDENT") == 0) {
              tcp_client_type = CLIENT_IS_STUDENT;
            } else if (strcmp(r_msg.substr(0, 15).c_str(), "I_AM_DEPARTMENT") == 0) {
              tcp_client_type = CLIENT_IS_DEPARTMENT;
            } else {
              std::cerr << "Unrecognized introduction: " << r_msg << "\n";
            }
            
            if (tcp_client_type > 0) {
              if (PROJ_DEBUG) {
                std::cout << "tcp_client_type set to " << tcp_client_type << "\n";
              }
              if (send(new_fd, "ADM_I_RCGZ", 10, 0) == -1) {
                perror("adm_recogniztion");
              }
            }
          } else {
            int can_continue = 0;
            if (PROJ_DEBUG) {
              std::cout << "Entering core communication phase with tcp_client_type " << tcp_client_type << "\n";
            }
            
            switch (tcp_client_type) {
              case CLIENT_IS_STUDENT:
                can_continue = handle_student_messages(new_fd,
                                                       r_msg,
                                                       &current_student,
                                                       &matching_program);
                break;
              case CLIENT_IS_DEPARTMENT:
                can_continue = handle_department_messages(new_fd,
                                                          r_msg.c_str(),
                                                          &current_dept);
                break;
              default:
                std::cerr << "tcp_client_type is not set: " << tcp_client_type << "\n";
            }
            
            if (can_continue == 0) {
              if (tcp_client_type == CLIENT_IS_STUDENT) {
                
                if (matching_program == 0) {
                  send(new_fd, "0", 1, 0);
                } else {
                  send(new_fd, "valid", 5, 0);
                }
              }
              break;
            }
          }
        }
      }
      
      close(new_fd);
      exit(0);
    } else {
      wait(NULL);
    }
    
    close(new_fd);
    if (db->check_is_complete()) {
      am->display_phase1_completed();
      if (ENABLE_PHASE_2) {
        am->redisplay_tcp_ip();
      }
      
      break;
    }
  }
}

int handle_student_messages(int new_fd, std::string msg, char *current_student, int *matching_program) {
  if (strcmp(msg.c_str(), "TX_FIN") == 0) {
    if (PROJ_DEBUG) {
      std::cout << "TX_FIN signal received. Closing socket\n";
    }
    
    am->display_student_completed(*current_student);
    return NO_MORE_ITERATION;
  } else {
    int rand_wait = rand() % 500 + 500;
    usleep(rand_wait);
    char interest[3] = "\0";
    
    int psm_error = process_student_message(msg, interest);
    
    if (psm_error == 0) {
      if (strlen(interest) > 0 && db->has_program(interest)) {
        (*matching_program)++;
      }
      *current_student = receive_buffer[0];
      
      if (send(new_fd, "ADM_RX_OK", 9, 0) == -1) {
        perror("student ack");
      } else {
        
      }
    }
    
    return PROCEED_WITH_ITERATION;
  }
}
                       
int handle_department_messages(int new_fd, const char *msg, char *current_dept) {
  if (strcmp(msg, "TX_FIN") == 0) {
    if (PROJ_DEBUG) {
      std::cout << "TX_FIN signal received. Closing socket " << new_fd << "\n";
    }
    
    am->display_department_completed(*current_dept);
    return NO_MORE_ITERATION;
  } else {
    int rand_wait = rand() % 500 + 500;
    usleep(rand_wait);
    int pdm_error;

    pdm_error = process_department_message(msg);
    
    if (pdm_error == 0) {
      if (PROJ_DEBUG) {
        std::cout << "No PDM error\n";
      }
      *current_dept = receive_buffer[0];
      
      if (PROJ_DEBUG) {
        std::cout << "Setting currend dept to " << *current_dept << "\n";
      }
      if (send(new_fd, "ADM_RX_OK", 9, 0) == -1) {
        perror("dept ack");
      } else {
        if (PROJ_DEBUG) {
          std::cout << "ADM_RX_OK sent out after randomly waiting for " << rand_wait << " us\n";
        }
      }
    }
    
    return PROCEED_WITH_ITERATION;
  }
}

uint32_t process_student_message(std::string _buffer, char *interest_s) {
  char *element;
  uint32_t e_pos = 0;
  char *buffer = new char[_buffer.length() + 1];
  strcpy(buffer, _buffer.c_str());
  element = strtok((char*) buffer, "#");
  int student_id = -1;
  bool parse_gpa = false;
  std::string interest = "";
  std::ofstream f;
  
  while (element != NULL) {
    switch(e_pos) {
      case 0:
        student_id = std::atoi(element);
        if (PROJ_DEBUG) {
          std::cout << "Captured student ID: " << student_id << "\n";
        }
        break;
      case 1:
        if (strcmp(element, "GPA") == 0) {
          interest_s[0] = '\0';
          parse_gpa = true;
        } else {
          interest = element;
          interest_s[0] = element[0];
          interest_s[1] = element[1];
          interest_s[2] = '\0';
          if (PROJ_DEBUG) {
            std::cout << "Capture interest of Student " << student_id << ": " << interest << "\n";
          }
        }
        break;
      case 2:
        if (parse_gpa) {
          student_gpa = std::atof(element);
          if (PROJ_DEBUG) {
            std::cout << "Captured GPA of Student " << student_id << ": " << student_gpa << "\n";
          }
        }
        break;
      default:
        std::cerr << "Shouldn't reach here\n";
    }
    
    element = strtok(NULL, "#");
    e_pos++;
  }
  
  f.open(DATABASE_FILE, std::ios::app);
  char line_buffer[MAXDATASIZE];
  if (parse_gpa) {
    sprintf(line_buffer, "S%d#GPA#%.1f\n", student_id, student_gpa);
  } else {
    sprintf(line_buffer, "S%d#%s\n", student_id, interest.c_str());
  }
  f << line_buffer;
  f.close();
  return 0;
}

uint32_t process_department_message(std::string _buffer) {
  char *element;
  uint32_t e_pos = 0;
  char *buffer = new char[_buffer.length() + 1];
  strcpy(buffer, _buffer.c_str());
  element = strtok((char*) buffer, "#");
  std::string dept_program = "";
  float dept_program_min_gpa = 0;
  
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
  
  std::ofstream f;
  f.open(DATABASE_FILE, std::ios::app);
  char line_buffer[MAXDATASIZE];
  
  sprintf(line_buffer, "%s#%.1f\n", dept_program.c_str(), dept_program_min_gpa);
  f << line_buffer;
  f.close();
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

std::string get_client_ip_address(int sockfd) {
  struct sockaddr_storage addr;
  char ipstr[INET6_ADDRSTRLEN];
  socklen_t addrlen = sizeof(addr);
  
  getpeername(sockfd, (struct sockaddr *) &addr, &addrlen);
  struct sockaddr_in *s = (struct sockaddr_in *) &addr;
  
  inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof(ipstr));
  return (std::string) ipstr;
}

