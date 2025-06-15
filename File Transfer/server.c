#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SIZE 1024

void send_file(int sockfd){
  FILE *fp;
  char *filename = "server_file.txt";
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
  char *filename = "received_from_client.txt";
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
  char buffer[SIZE];
  int port = 8080;
  int sockfd, new_sock;
  struct sockaddr_in server_addr, new_addr;
  socklen_t addr_size;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
    perror("[-]Bind failed");
    exit(1);
  }
  printf("[+]Bind to port %d\n", port);

  if (listen(sockfd, 10) == 0){
    printf("[+]Listening...\n");
  }

  addr_size = sizeof(new_addr);
  new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
  printf("[+]Client connected.\n");

  bzero(buffer, SIZE);
  recv(new_sock, buffer, SIZE, 0);

  if (strcmp(buffer, "upload") == 0) {
    write_file(new_sock);
    printf("[+]File received from client and saved as received_from_client.txt\n");

  } else if (strcmp(buffer, "download") == 0) {
    send(new_sock, "OK", 2, 0); 
    send_file(new_sock);
    printf("[+]File sent to client from server_file.txt\n");

  } else {
    printf("[-]Unknown command received.\n");
  }

  close(new_sock);
  close(sockfd);

  return 0;
}
