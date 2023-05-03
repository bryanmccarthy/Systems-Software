#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char *argv[]) {
  
  int sock = 0, valread;
  struct sockaddr_in serv_addr;
  char buffer[1024] = {0};
  int menu_choice;
  char *transfer = "transfer";

  // Create socket file descriptor
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n Socket creation error \n");
    return -1;
  }

  // Set address family to IPv4
  serv_addr.sin_family = AF_INET;
  // Set address to localhost
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  // Set port to 8080
  serv_addr.sin_port = htons(PORT);

  // Connect to server
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("\nConnection Failed \n");
    return -1;
  }

  while(1) {
    printf("\n1: Transfer file \n2: Exit \n");
    scanf("%d", &menu_choice);

    switch(menu_choice) {
      case 1:
        send(sock, transfer, strlen(transfer), 0);
        break;
      case 2:
        printf("exiting\n");
        exit(0);
      default:
        printf("invalid choice\n");
    }
  }

  close(sock);
  
  return 0;
}
