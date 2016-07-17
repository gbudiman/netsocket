//
//  Admission.cpp
//  netsocket
//
//  Created by Gloria Budiman on 7/16/16.
//  Copyright © 2016 gbudiman. All rights reserved.
//

#include <stdio.h>
#include "main.h"
#include "Admission.h"

void sigchld_handler(int s) {
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*) sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

int main() {
  int sockfd, new_fd;
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  struct sigaction sa;
  int yes = 1;
  char s[INET6_ADDRSTRLEN];
  char receive_buffer[MAXDATASIZE];
  int receive_length;
  int rv;
  
  memset(&hints, 0 ,sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  
  if ((rv = getaddrinfo(NULL, ADMISSION_PORT, &hints, &servinfo)) != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(rv);
    return 1;
  }
  
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }
    
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }
    
    break;
  }
  
  if (p == NULL) {
    std::cerr << "Admission Server: failed to bind\n";
    return 2;
  }
  
  freeaddrinfo(servinfo);
  
  if (listen(sockfd, ADMISSION_BACKLOG) == -1) {
    perror("listen");
    exit(-1);
  }
  
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }
  
  std::cout << "Admission Server: waiting for connections...\n";
  
  while(1) {
    sin_size = sizeof(their_addr);
    new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }
    
    inet_ntop(their_addr.ss_family,
              get_in_addr((struct sockaddr *) &their_addr),
              s, sizeof s);
    std::cout << "Admission Server: got connection from " << s << "\n";
    
    if (!fork()) {
      close(sockfd);
      if (send(new_fd, "Hello, child!", 13, 0) == -1) {
        perror("send");
      }
      
      int receive_length = recv(new_fd, receive_buffer, MAXDATASIZE - 1, 0);
      debug_receive_buffer(receive_buffer, receive_length);
      close(new_fd);
      exit(0);
    }
    close(new_fd);
  }
  
  return 0;
}

void debug_receive_buffer(char *receive_buffer, int receive_length) {
  std::cout << "Received message (" << receive_length << " bytes): ";
  for (int i = 0; i < receive_length; i++) {
    printf("%c", receive_buffer[i]);
  }
  
  std::cout << "\n";
}

