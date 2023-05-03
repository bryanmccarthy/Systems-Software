#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

int main(int argc, char *argv[]) {
  
  int server_fd, client_fd, opt = 1;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};

  // Create socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Attach socket to port 8080
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  // Set address family to IPv4
  address.sin_family = AF_INET;
  
  // Set address to localhost
  address.sin_addr.s_addr = INADDR_ANY;

  // Set port to 8080
  address.sin_port = htons(PORT);

  // Bind socket to address and port
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  // Listen for connections
  if (listen(server_fd, 3) < 0) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  // Server loop
  while (1) {

    if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
      perror("accept failed");
      exit(EXIT_FAILURE);
    }

    printf("Connection made, client address: %s\n", inet_ntoa(address.sin_addr));
    
    // Read from client
    read(client_fd, buffer, 1024);

    printf("Message from client: %s\n", buffer);
    
  }
  
  close(client_fd);

  close(server_fd);

  return 0;
}
