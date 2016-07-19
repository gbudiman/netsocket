#ifndef AdmissionMessenger_hpp
#define AdmissionMessenger_hpp

#include "main.h"

#define AMSG_P1_START 0
#define AMSG_DEPT_COMPLETED 1
#define AMSG_P1_END 2
#define AMSG_P2_START 3

class AdmissionMesseger {
public:
  AdmissionMesseger();
  void display_tcp_ip(addrinfo*, char*);
  void display_department_completed(char);
  void display_phase1_completed();
private:
  void display(int);
  void display(int, std::vector<std::string>*);
};

void *get_in_addr(struct sockaddr *sa);
#endif
