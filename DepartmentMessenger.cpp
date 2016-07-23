#include "DepartmentMessenger.hpp"



DepartmentMessenger::DepartmentMessenger() {
  
}

void DepartmentMessenger::set_department_name(std::string x) {
  department_name = x;
}

void DepartmentMessenger::display_tcp_ip(std::string port, std::string ip_address) {
  std::string dept_name_s = "";
  std::vector<std::string> *args = new std::vector<std::string>();
  
  args->push_back(ip_address);
  args->push_back(port);
  
  display(DMSG_P1_START, args);
}

void DepartmentMessenger::display_connected() {
  display(DMSG_ADM_CONNECTED);
}

void DepartmentMessenger::display_phase1_completed() {
  display(DMSG_P1_END);
}

void DepartmentMessenger::display_all_program_sent() {
  display(DMSG_PROG_COMPLETED);
}

void DepartmentMessenger::display_one_program_sent(std::string s) {
  std::vector<std::string> *args = new std::vector<std::string>;
  args->push_back(s);
  
  display(DMSG_PROG_SENT, args);
}

void DepartmentMessenger::display(int type) {
  DepartmentMessenger::display(type, NULL);
}

void DepartmentMessenger::display(int type, std::vector<std::string> *args) {
  const char *d = department_name.c_str();
  switch(type) {
    case DMSG_P1_START:
      std::cout << "Department " << d
      << " has TCP port " << args->at(1)
      << " and IP " << args->at(0) << "\n";
      break;
    case DMSG_ADM_CONNECTED:
      std::cout << "Department" << d << " is now connected to the admission office\n";
      break;
    case DMSG_PROG_SENT:
      std::cout << "Department" << d << " has sent " << args->at(0) << " to the admission office\n";
      break;
    case DMSG_PROG_COMPLETED:
      std::cout << "Updating the admission office is done for Department " << d << "\n";
      break;
    case DMSG_P1_END:
      std::cout << "End of Phase 1 for Department " << d << "\n";
      break;
  }
}
