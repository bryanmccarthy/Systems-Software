#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>

#define PORT 8080

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
  pthread_mutex_t lock;
  int valread;
  char department[20] = {0};
  char username[20] = {0};
  uid_t uid;
  char file_name[100] = {0};
  char time_str[80];
  char success_msg[100] = "File transfer successful\n";
  char failure_msg[100] = "File transfer failed\n";

  while(1) {

    valread = read(client_fd, buffer, 1024);
    if (valread == 0) {
      break;
    }

    printf("Command from client: %s\n", buffer);

    if(strcmp(buffer, "exit") == 0) {
      break;
    }

    // Manufacturing command for department
    if(strcmp(buffer, "manufacturing") == 0) {
      strcpy(department, "manufacturing");
    }

    // Distribution command for department
    if(strcmp(buffer, "distribution") == 0) {
      strcpy(department, "distribution");
    }

    // Transfer command
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

      // Get uid
      valread = read(client_fd, buffer, 1024);
      if (valread == 0) {
        break;
      }

      uid = *(uid_t *)buffer;
      printf("UID: %d\n", uid);

      // clear buffer
      memset(buffer, 0, 1024);

      // Get current time
      get_time(time_str);

      // Create file in department directory
      char file_path[100] = {0};
      sprintf(file_path, "%s/%s", department, file_name);

      // Lock mutex
      pthread_mutex_lock(&lock);

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

      // Unlock mutex
      pthread_mutex_unlock(&lock);

      printf("Transfer complete\n");

      // Report the transfer (username, file name, department, time) in report/report.txt
      char report_path[100] = "report/report.txt";

      // Lock mutex
      pthread_mutex_lock(&lock);

      FILE *report_fp = fopen(report_path, "a");
      if(report_fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
      }

      fprintf(report_fp, "%s - %d - %s - %s %s\n", username, uid, file_name, department, time_str);

      fclose(report_fp);

      // Unlock mutex
      pthread_mutex_unlock(&lock);

      printf("Report updated\n");

      // Inform client if transfer was successful or not
      if(access(file_path, F_OK) != -1) {
        write(client_fd, success_msg, strlen(success_msg));
      } else {
        write(client_fd, failure_msg, strlen(failure_msg));
      }
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

  // Bind socket to address
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
  
  close(server_fd);
  return 0;
}
