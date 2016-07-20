#include "Socket.hpp"

std::string Socket::get_socket_port(int sockfd) {
  struct sockaddr_in sin;
  socklen_t addrlen = sizeof(sin);
  getsockname(sockfd, (struct sockaddr *) &sin, &addrlen);
  
  int local_port = ntohs(sin.sin_port);
  char local_port_s[MAXDATASIZE];
  sprintf(local_port_s, "%d", local_port);
  return (std::string) local_port_s;
}

std::string Socket::get_self_ip_address() {
  struct addrinfo hints, *res, *p;
  char ipstr[INET6_ADDRSTRLEN];
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  
  getaddrinfo(SERVER, NULL, &hints, &res);
  
  for (p = res; p != NULL; p = p->ai_next) {
    void *addr;
    struct sockaddr_in *ip = (struct sockaddr_in *) p->ai_addr;
    addr = &(ip->sin_addr);
    
    inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
  }
  
  freeaddrinfo(res);
  return (std::string) ipstr;
}

void* get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*) sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*) sa)->sin6_addr);
}
