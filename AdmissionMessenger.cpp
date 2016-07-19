#include "AdmissionMessenger.hpp"

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*) sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

AdmissionMesseger::AdmissionMesseger() {
  
}

void AdmissionMesseger::display_tcp_ip(int sockfd, std::string ip) {
  struct sockaddr_in sin;
  socklen_t addrlen = sizeof(sin);
  getsockname(sockfd, (struct sockaddr *) &sin, &addrlen);
  int local_port = ntohs(sin.sin_port);
  char local_port_s[MAXDATASIZE];
  sprintf(local_port_s, "%d", local_port);
  
  struct sockaddr_storage my_addr;
  char host_ip[255];
  std::vector<std::string> *args = new std::vector<std::string>();
  
  //inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) &p), host_ip, sizeof(host_ip));
  args->push_back(ip);
  args->push_back((std::string) local_port_s);
  
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
