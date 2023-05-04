#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>

#define PORT 8080

// Function to get current time
void get_time(char *time_str) {
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(time_str, 80, "%d-%m-%Y %I:%M:%S", timeinfo);
}

void *handle_client(void *arg) {

  int client_fd = *(int *)arg;
  char buffer[1024] = {0};
  int valread;
  // department
  char department[20] = {0};
  char username[20] = {0};
  char file_name[100] = {0};
  char time_str[80];

  while(1) {

    valread = read(client_fd, buffer, 1024);

    if (valread == 0) {
      break;
    }

    printf("Message from client: %s\n", buffer);

    if(strcmp(buffer, "exit") == 0) {
      break;
    }

    // if buffer = "manufacturing" 
    if(strcmp(buffer, "manufacturing") == 0) {
      strcpy(department, "manufacturing");
    }

    // if buffer = "distribution"
    if(strcmp(buffer, "distribution") == 0) {
      strcpy(department, "distribution");
    }

    if(strcmp(buffer, "transfer") == 0) {
      printf("\n%s transfer started\n", department);

      // clear buffer
      memset(buffer, 0, 1024);

      // Get file name
      valread = read(client_fd, buffer, 1024);
      if (valread == 0) {
        break;
      }

      strcpy(file_name, buffer);
      printf("File name: %s\n", buffer);

      // clear buffer
      memset(buffer, 0, 1024);

      // Get username
      valread = read(client_fd, buffer, 1024);
      if (valread == 0) {
        break;
      }

      strcpy(username, buffer);
      printf("Username: %s\n", username);

      // clear buffer
      memset(buffer, 0, 1024);

      // Get current time
      get_time(time_str);

      // Create file in department directory
      char file_path[100] = {0};
      sprintf(file_path, "%s/%s", department, file_name);

      FILE *fp = fopen(file_path, "w");
      if(fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
      }

      // Get file contents
      valread = read(client_fd, buffer, 1024);
      if (valread == 0) {
        break;
      }

      // Write file contents to file
      fprintf(fp, "%s", buffer);

      // Close file
      fclose(fp);

      printf("Transfer complete\n");

      // Report the transfer (username, file name, department, time) in report/report.txt
      char report_path[100] = "report/report.txt";

      FILE *report_fp = fopen(report_path, "a");
      if(report_fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
      }

      fprintf(report_fp, "%s %s %s %s\n", username, file_name, department, time_str);

      fclose(report_fp);

      printf("Report updated\n");
    }

    // clear buffer
    memset(buffer, 0, 1024);
  }

  close(client_fd);

  return NULL;
}

int main(int argc, char *argv[]) {
  
  int server_fd, client_fd, opt = 1;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};

  // Create socket file descriptor
  if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Attach socket to port 8080
  if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
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
  if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  // Listen for connections
  if(listen(server_fd, 3) < 0) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  printf("Listening on port %d\n", PORT);

  // Server loop
  while(1) {

    printf("Waiting for connection...\n");

    if((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
      perror("accept failed");
      exit(EXIT_FAILURE);
    }

    printf("Connection made, client address: %s\n", inet_ntoa(address.sin_addr));
    
    // Create thread to handle client
    pthread_t thread;
    if(pthread_create(&thread, NULL, handle_client, (void *)&client_fd) < 0) {
      perror("pthread_create failed");
      exit(EXIT_FAILURE);
    }
  }
  
  close(client_fd);

  close(server_fd);

  return 0;
}
