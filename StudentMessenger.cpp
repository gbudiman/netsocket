#include "StudentMessenger.hpp"

StudentMessenger::StudentMessenger() {
  
}

void StudentMessenger::set_student_name(int x) {
  student_name = x;
}

void StudentMessenger::display_tcp_ip(std::string port, std::string ip_address) {
  std::vector<std::string> *args = new std::vector<std::string>();
  
  args->push_back(ip_address);
  args->push_back(port);
  
  display(SMSG_P2_TCP_START, args);
}

void StudentMessenger::display_udp_ip(std::string port, std::string ip_address) {
  std::vector<std::string> *args = new std::vector<std::string>();
  
  args->push_back(ip_address);
  args->push_back(port);
  
  display(SMSG_P2_UDP_START, args);
}

void StudentMessenger::display_received_application_result() {
  display(SMSG_RECEIVED_RESULT);
}

void StudentMessenger::display_all_applications_sent() {
  display(SMSG_APPLICATION_COMPLETED);
}

void StudentMessenger::display_p2_end() {
  display(SMSG_P2_END);
}

void StudentMessenger::display(int type) {
  StudentMessenger::display(type, NULL);
}

void StudentMessenger::display(int type, std::vector<std::string> *args) {
  switch(type) {
    case SMSG_P2_TCP_START:
      std::cout << "Student" << student_name << " has TCP port " << args->at(1) << " and IP address " << args->at(0) << "\n";
      break;
    case SMSG_APPLICATION_COMPLETED:
      std::cout << "Completed sending application for Student" << student_name << "\n";
      break;
    case SMSG_RECEIVED_REPLY:
      std::cout << "Student" << student_name << " has received the reply from the admission office\n";
      break;
    case SMSG_P2_UDP_START:
      std::cout << "Student" << student_name << " has UDP port " << args->at(1) << " and IP address " << args->at(0) << "\n";
      break;
    case SMSG_RECEIVED_RESULT:
      std::cout << "Student" << student_name << " has received application result\n";
      break;
    case SMSG_P2_END:
      std::cout << "End of phase 2 for Student" << student_name << "\n";
      break;
  }
}
