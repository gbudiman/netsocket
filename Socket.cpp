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
  hints.ai_flags = AI_PASSIVE;
  
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

int Socket::create_socket(int type) {
  int sockfd = 0;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  
  if (type == TCP_SERVER) {
    hints.ai_flags = AI_PASSIVE;
  }
  
  if ((rv = getaddrinfo(SERVER, ADMISSION_PORT, &hints, &servinfo)) != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(rv) << "\n";
    return 1;
  }
  
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      //perror("client: socket");
      continue;
    }
    
    if (type == TCP_CLIENT) {
      if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd);
        continue;
      }
    } else if (type == TCP_SERVER) {
      int yes = 1;
      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        exit(1);
      }
      if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd);
        continue;
      }
    }
    
    break;
  }
  
  if (p == NULL) {
    std::cerr << "Failed to connect\n";
    return -2;
  }
  
  
  freeaddrinfo(servinfo);
  
  return sockfd;
}

void* get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*) sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*) sa)->sin6_addr);
}
