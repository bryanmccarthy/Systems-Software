#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <grp.h>
#include <stdbool.h>

#define PORT 8080

// Function to send file to server
void file_transfer(int sock, char buffer[], char username[], uid_t uid) {
  char file_path[100];
  char file_name[100];

  // Get file path
  printf("Enter file path: ");
  scanf("%s", file_path);

  if(access(file_path, F_OK) == -1) {
    printf("File does not exist\n");
    return;
  }

  // Get file name
  printf("Enter file name: ");
  scanf("%s", file_name);

  /* Start file transfer */
  // Send transfer command to server
  send(sock, "transfer", strlen("transfer"), 0);
  memset(buffer, 0, 1024);
  sleep(1);

  // Send file name to server
  send(sock, file_name, strlen(file_name), 0);
  memset(buffer, 0, 1024);
  sleep(1);

  // Send username to server
  send(sock, username, strlen(username), 0);
  memset(buffer, 0, 1024);
  sleep(1);

  // Send uid to server
  send(sock, &uid, sizeof(uid), 0);
  memset(buffer, 0, 1024);
  sleep(1);

  // Open file
  FILE *fp = fopen(file_path, "r");

  // Send file contents to server
  while(fgets(buffer, 1024, fp) != NULL) {
    send(sock, buffer, strlen(buffer), 0);
    memset(buffer, 0, 1024);
  }

  // Close file 
  fclose(fp);
  memset(buffer, 0, 1024);

  // Receive transfer status from server
  read(sock, buffer, 1024);
  printf("%s\n", buffer);
  memset(buffer, 0, 1024);
}

int main(int argc, char *argv[]) {
  
  int sock = 0, valread;
  struct sockaddr_in serv_addr;
  char buffer[1024] = {0};
  int menu_choice;
  int ngroups;
  gid_t *groups;

  char departments[2][20] = {"manufacturing", "distribution"};
  bool is_manufacturing = false;
  bool is_distribution = false;

  char *username = getlogin();
  uid_t uid = getuid();

  printf("Username: %s\n", username);
  printf("UID: %d\n", uid);

  // Get number of groups
  ngroups = getgroups(0, NULL);

  // Allocate memory for groups
  groups = malloc(ngroups * sizeof(gid_t));

  // Get groups
  getgroups(ngroups, groups);

  // Check if user is in manufacturing or distribution group
  for (int i = 0; i < ngroups; i++) {
    struct group *grp = getgrgid(groups[i]);

    if (strcmp(grp->gr_name, departments[0]) == 0) {
      is_manufacturing = true;
    }

    if (strcmp(grp->gr_name, departments[1]) == 0) {
      is_distribution = true;
    }
  }

  free(groups);

  // Create socket file descriptor
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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
  if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("\nConnection Failed \n");
    return -1;
  }

  // Client loop
  while(1) {
    printf("\nMenu:");
    printf("\n1: Transfer file \n2: Exit \n");
    scanf("%d", &menu_choice);

    switch(menu_choice) {
      case 1:
        printf("\nSelect department:");
        printf("\n1: Manufacturing \n2: Distribution \n");
        scanf("%d", &menu_choice);

        switch(menu_choice) {
          case 1:
            if(!is_manufacturing) {
              printf("You are not in the manufacturing department\n");
              break;
            }

            // Send manufacturing command to server
            send(sock, "manufacturing", strlen("manufacturing"), 0);
            
            // clear buffer
            memset(buffer, 0, 1024);

            // Start file transfer
            file_transfer(sock, buffer, username, uid);
            
            break;
          case 2:
            if(!is_distribution) {
              printf("You are not in the distribution department\n");
              break;
            }

            // Send distribution command to server
            send(sock, "distribution", strlen("distribution"), 0);
            
            // clear buffer
            memset(buffer, 0, 1024);

            // Start file transfer
            file_transfer(sock, buffer, username, uid);

            break;
          default:
            printf("invalid choice\n");
        }

        break;

      case 2:
        printf("exiting\n");
        send(sock, "exit", strlen("exit"), 0);
        exit(0);

      default:
        printf("invalid choice\n");
    }
  }

  close(sock);
  
  return 0;
}
