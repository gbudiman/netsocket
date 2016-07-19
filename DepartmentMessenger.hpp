#ifndef DepartmentMessenger_hpp
#define DepartmentMessenger_hpp

#include "main.h"
#include "Socket.hpp"

#define DMSG_P1_START 0
#define DMSG_ADM_CONNECTED 1
#define DMSG_PROG_SENT 2
#define DMSG_PROG_COMPLETED 3
#define DMSG_P1_END 4
#define DMSG_P2_START 5
#define DMSG_STUDENT_RECEIVED 6
#define DMSG_P2_END 7

class DepartmentMessenger {
public:
  DepartmentMessenger();
  void set_department_name(std::string);
  void display_tcp_ip(addrinfo*, char*);
  void display_connected();
  void display_one_program_sent(std::string);
  void display_all_program_sent();
  void display_phase1_completed();
private:
  void display(int);
  void display(int, std::vector<std::string>*);
  
  std::string department_name;
};

#endif
