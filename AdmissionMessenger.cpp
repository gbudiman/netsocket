#include "AdmissionMessenger.hpp"

AdmissionMesseger::AdmissionMesseger() {
}

void AdmissionMesseger::display_tcp_ip(std::string port, std::string ip) {
  std::vector<std::string> *args = new std::vector<std::string>();
  
  args->push_back(ip);
  args->push_back(port);
  
  reuse_tcp_port = port;
  reuse_ip_address = ip;
  
  display(AMSG_P1_START, args);
}

void AdmissionMesseger::display_udp_ip(std::string port, std::string ip) {
  std::vector<std::string> *args = new std::vector<std::string>();
  
  args->push_back(ip);
  args->push_back(port);
  
  reuse_tcp_port = port;
  reuse_ip_address = ip;
  
  display(AMSG_UDP_CREATED, args);
}

void AdmissionMesseger::redisplay_tcp_ip() {
  std::vector<std::string> *args = new std::vector<std::string>();
  
  args->push_back(reuse_ip_address);
  args->push_back(reuse_tcp_port);
  
  display(AMSG_P2_START, args);
}

void AdmissionMesseger::display_department_completed(char dept_name) {
  std::vector<std::string> *args = new std::vector<std::string>();
  std::string dept_name_s = "";
  dept_name_s = dept_name;
  
  args->push_back(dept_name_s);
  display(AMSG_DEPT_COMPLETED, args);
}

void AdmissionMesseger::display_department_admission_result(char dept_name) {
  std::vector<std::string> *args = new std::vector<std::string>();
  std::string dept_name_s = "";
  dept_name_s = dept_name;
  
  args->push_back(dept_name_s);
  display(AMSG_DEPARTMENT_RESULT, args);
}

void AdmissionMesseger::display_phase1_completed() {
  display(AMSG_P1_END);
}

void AdmissionMesseger::display_phase2_completed() {
  display(AMSG_P2_END);
}

void AdmissionMesseger::display_student_completed(char x) {
  std::vector<std::string> *args = new std::vector<std::string>();
  std::string student_name = "";
  student_name += x;
  
  args->push_back(student_name);
  display(AMSG_STUDENT_COMPLETED, args);
}

void AdmissionMesseger::display_student_admission_result(char x) {
  std::vector<std::string> *args = new std::vector<std::string>();
  std::string student_name = "";
  student_name += x;
  
  args->push_back(student_name);
  display(AMSG_STUDENT_RESULT, args);
}

void AdmissionMesseger::display(int type) {
  display(type, NULL);
}

void AdmissionMesseger::display(int type, std::vector<std::string> *args) {
  switch(type) {
    case AMSG_P1_START:
    case AMSG_P2_START:
      std::cout << "The admission office has TCP port "
      << args->at(1)
      << " and IP address " << args->at(0) << "\n";
      break;
    case AMSG_DEPT_COMPLETED:
      std::cout << "Received the program list from Department"
      << args->at(0) << "\n";
      break;
    case AMSG_P1_END:
      std::cout << "End of Phase 1 for the admission office\n";
      break;
    case AMSG_STUDENT_COMPLETED:
      std::cout << "Admission office receive the application from Student"
      << args->at(0)
      << "\n";
      break;
    case AMSG_UDP_CREATED:
      std::cout << "The admission office has UDP port "
      << args->at(1)
      << " and IP "
      << args->at(0)
      << " for Phase 2\n";
      break;
    case AMSG_STUDENT_RESULT:
      std::cout << "The admission office has send the application result to Student"
      << args->at(0)
      << "\n";
      break;
    case AMSG_DEPARTMENT_RESULT:
      std::cout << "The admission office has send one admitted student to Department"
      << args->at(0)
      << "\n";
      break;
    case AMSG_P2_END:
      std::cout << "End of Phase 2 for the admission office\n";
      break;
  }
}
