#ifndef AdmissionMessenger_hpp
#define AdmissionMessenger_hpp

#include "main.h"

#define AMSG_P1_START 0
#define AMSG_DEPT_COMPLETED 1
#define AMSG_P1_END 2
#define AMSG_P2_START 3
#define AMSG_STUDENT_COMPLETED 4
#define AMSG_UDP_CREATED 5
#define AMSG_STUDENT_RESULT 6
#define AMSG_DEPARTMENT_RESULT 7
#define AMSG_P2_END 8

class AdmissionMesseger {
public:
  AdmissionMesseger();
  void display_tcp_ip(std::string, std::string);
  void display_udp_ip(std::string, std::string);
  void display_department_completed(char);
  void display_student_completed(char);
  void display_student_admission_result(char);
  void display_department_admission_result(char);
  void display_phase1_completed();
  void display_phase2_completed();
  void redisplay_tcp_ip();
private:
  void display(int);
  void display(int, std::vector<std::string>*);
  
  std::string reuse_tcp_port;
  std::string reuse_ip_address;
};

#endif
