#ifndef AdmissionMessenger_hpp
#define AdmissionMessenger_hpp

#include "main.h"

#define AMSG_P1_START 0
#define AMSG_DEPT_COMPLETED 1
#define AMSG_P1_END 2
#define AMSG_P2_START 3
#define AMSG_STUDENT_COMPLETED 4

class AdmissionMesseger {
public:
  AdmissionMesseger();
  void display_tcp_ip(std::string, std::string);
  void display_department_completed(char);
  void display_student_completed(char);
  void display_phase1_completed();
  void redisplay_tcp_ip();
private:
  void display(int);
  void display(int, std::vector<std::string>*);
  
  std::string reuse_tcp_port;
  std::string reuse_ip_address;
};

#endif
