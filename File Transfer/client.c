#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SIZE 1024

void send_file(int sockfd){
  FILE *fp;
  char *filename = "client_file.txt";
  char data[SIZE];
  size_t n;

  fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("[-]Error in reading file.");
    exit(1);
  }

  while ((n = fread(data, 1, SIZE, fp)) > 0) {
    if (send(sockfd, data, n, 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, SIZE);
  }
  fclose(fp);
}

void write_file(int sockfd){
  int n;
  FILE *fp;
  char *filename = "downloaded_from_server.txt";
  char buffer[SIZE];

  fp = fopen(filename, "w");
  while ((n = recv(sockfd, buffer, SIZE, 0)) > 0) {
    fwrite(buffer, 1, n, fp);
    bzero(buffer, SIZE);
    if (n < SIZE)
      break;  
  }
  fclose(fp);
}

int main(){
  char *ip = "127.0.0.1";
  int port = 8080;
  int sockfd;
  struct sockaddr_in server_addr;
  char buffer[SIZE];
  char command[10];

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Client socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);

  if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    perror("[-]Error in connection");
    exit(1);
  }
  printf("[+]Connected to Server.\n");

  printf("Enter command (upload / download): ");
  scanf("%s", command);

  send(sockfd, command, strlen(command), 0);

  if (strcmp(command, "upload") == 0) {
    send_file(sockfd);
    printf("[+]File uploaded to server.\n");

  } else if (strcmp(command, "download") == 0) {
    recv(sockfd, buffer, SIZE, 0);  
    if (strncmp(buffer, "OK", 2) == 0) {
      write_file(sockfd);
      printf("[+]File downloaded from server.\n");
    } else {
      printf("[-]Server failed to start file transfer.\n");
    }

  } else {
    printf("[-]Unknown command.\n");
  }

  close(sockfd);
  return 0;
}
