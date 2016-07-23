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
  create_empty_database_file();
  create_tcp_and_process();
  make_admission_decision();
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
}

void create_tcp_and_process() {
  srand(time(NULL));
  database = new std::map<std::string, float>();
  interest_database = new std::vector<std::string>();
  
  int sockfd = 0, new_fd;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  
  int depts_completed = 0;
  int students_completed = 0;
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
                                                       &current_student);
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
      if (ENABLE_PHASE_2) {
        am->redisplay_tcp_ip();
      }
      
      break;
    }
  }
}

int handle_student_messages(int new_fd, std::string msg, char *current_student) {
  if (strcmp(msg.c_str(), "TX_FIN") == 0) {
    if (PROJ_DEBUG) {
      std::cout << "TX_FIN signal received. Closing socket\n";
    }
    
    am->display_student_completed(*current_student);
    return NO_MORE_ITERATION;
  } else {
    int rand_wait = rand() % 500 + 500;
    usleep(rand_wait);
    
    int psm_error = process_student_message(msg);
    
    if (psm_error == 0) {
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

uint32_t process_student_message(std::string _buffer) {
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
          parse_gpa = true;
        } else {
          interest = element;
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

bool check_department_completion(int *d_size, int *s_size) {
  std::cout << "D: " << *d_size << " S: " << *s_size << "\n";
  if (*d_size == NUM_DEPTS && *s_size == NUM_STUDENTS) {
    am->display_phase1_completed();
    
    *d_size = 0;
    *s_size = 0;
    
    return true;
  }
  
  if (*d_size > NUM_DEPTS) {
    *d_size -= NUM_DEPTS;
  }
  
  if (*s_size > NUM_STUDENTS) {
    *s_size -= NUM_STUDENTS;
  }
  
  return false;
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

