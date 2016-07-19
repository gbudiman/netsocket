#include "AdmissionMessenger.hpp"

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*) sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

AdmissionMesseger::AdmissionMesseger() {
  
}

void AdmissionMesseger::display_tcp_ip(std::string port, std::string ip) {
  std::vector<std::string> *args = new std::vector<std::string>();
  
  args->push_back(ip);
  args->push_back(port);
  
  display(AMSG_P1_START, args);
}

void AdmissionMesseger::display_department_completed(char dept_name) {
  std::vector<std::string> *args = new std::vector<std::string>();
  std::string dept_name_s = "";
  dept_name_s = dept_name;
  
  args->push_back(dept_name_s);
  display(AMSG_DEPT_COMPLETED, args);
}

void AdmissionMesseger::display_phase1_completed() {
  display(AMSG_P1_END);
}

void AdmissionMesseger::display(int type) {
  display(type, NULL);
}

void AdmissionMesseger::display(int type, std::vector<std::string> *args) {
  switch(type) {
    case AMSG_P1_START:
      std::cout << "The admission office has TCP port " << args->at(1)
      << " and IP " << args->at(0) << "\n";
      break;
    case AMSG_DEPT_COMPLETED:
      std::cout << "Received the program list from " << args->at(0) << "\n";
      break;
    case AMSG_P1_END:
      std::cout << "End of Phase 1 for the admission office\n";
  }
}
