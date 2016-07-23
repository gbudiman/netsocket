#ifndef Socket_hpp
#define Socket_hpp

#include "main.h"

class Socket {
public:
  static std::string get_socket_port(int);
  static std::string get_self_ip_address();
  static void* get_in_address(struct sockaddr);
  static int create_socket(int);
};

#endif
