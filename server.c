#include <stdio.h>              // Standard I/O operations
#include <stdlib.h>             // Standard library for exit()
#include <string.h>             // For string operations
#include <unistd.h>             // For close(), read(), write(), sleep()
#include <arpa/inet.h>          // For socket programming (inet_addr, sockaddr_in)
#define SIZE 1024               // Buffer size for data chunks
#define MAX_FILE_SIZE 5242880   // 5 MB max file size (5 * 1024 * 1024)

char filename[100];             // Global variable to store file name

// Function to send file to the connected socket
void send_file(int sockfd) {
  FILE *fp;
  char data[SIZE];
  size_t n;
  size_t total_bytes = 0;       // Track total bytes sent

  fp = fopen(filename, "r");   // Open the file for reading
  if (fp == NULL) {
    perror("[-]Error in reading file.");
    exit(1);
  }

  // Read file in chunks and send to socket
  while ((n = fread(data, 1, SIZE, fp)) > 0) {
    total_bytes += n;           // Add to total bytes
    if (total_bytes > MAX_FILE_SIZE) {
      printf("[-]File exceeds 5 MB size limit. Aborting.\n");
      fclose(fp);
      return;
    }
    if (send(sockfd, data, n, 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, SIZE);          // Clear buffer
  }
  fclose(fp);                   // Close the file
}

// Function to receive file and write to disk
void write_file(int sockfd) {
  int n;
  FILE *fp;
  char buffer[SIZE];
  char *save_as = "received_from_client.txt"; // Fixed output file name
  size_t total_bytes = 0;       // Track received bytes

  fp = fopen(save_as, "w");    // Open file to write received data
  while ((n = recv(sockfd, buffer, SIZE, 0)) > 0) {
    total_bytes += n;          // Track total file size
    if (total_bytes > MAX_FILE_SIZE) {
      printf("[-]Received file exceeds 5 MB. Stopping write.\n");
      break;
    }
    fwrite(buffer, 1, n, fp);  // Write buffer to file
    bzero(buffer, SIZE);       // Clear buffer
    if (n < SIZE)              // End of file transfer condition
      break;
  }
  fclose(fp);                  // Close the file
}

int main() {
  char buffer[SIZE];           // Buffer for temporary message exchange
  int port = 8080;             // Server port
  int sockfd, new_sock;        // Socket descriptors
  struct sockaddr_in server_addr, new_addr; // Socket address structures
  socklen_t addr_size;         // For accept()
  int choice;                  // To select between modes

  // Ask user to choose mode
  printf("Select mode:\n1 - Listening Mode\n2 - File Transfer Mode\nChoice: ");
  scanf("%d", &choice);

  sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket
  if (sockfd < 0){
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created.\n");

  // Configure server socket
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Bind socket to IP and port
  if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
    perror("[-]Bind failed");
    exit(1);
  }
  printf("[+]Bind to port %d\n", port);

  // Start listening for incoming connections
  if (listen(sockfd, 10) == 0){
    printf("[+]Listening...\n");
  }

  // Accept incoming connection
  addr_size = sizeof(new_addr);
  new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
  printf("[+]Client connected.\n");

  if (choice == 2) {
    char command[10];
    printf("Enter command (upload/download): ");
    scanf("%s", command);                     // Get user command
    send(new_sock, command, strlen(command), 0); // Send command to client
    sleep(1);                                  // Wait briefly

    if (strcmp(command, "upload") == 0) {
      printf("Enter file name to upload: ");
      scanf("%s", filename);                  // Get file name
      send(new_sock, filename, strlen(filename), 0); // Send filename
      sleep(1);                                // Wait briefly
      send_file(new_sock);                     // Upload file
      printf("[+]File uploaded to client.\n");

    } else if (strcmp(command, "download") == 0) {
      printf("Enter file name to download: ");
      scanf("%s", filename);                  // Get file name
      send(new_sock, filename, strlen(filename), 0); // Send filename
      sleep(1);                                // Wait briefly
      write_file(new_sock);                    // Receive file
      printf("[+]File downloaded from client.\n");

    } else {
      printf("[-]Unknown command.\n");         // Invalid command
    }

  } else if (choice == 1) {
    bzero(buffer, SIZE);                       // Clear buffer
    recv(new_sock, buffer, SIZE, 0);           // Receive command from client
    char command[10];
    strcpy(command, buffer);                   // Copy command to variable

    if (strcmp(command, "upload") == 0) {
      bzero(filename, 100);                    // Clear filename buffer
      recv(new_sock, filename, 100, 0);        // Receive filename from client
      printf("[*]Client is uploading file: %s\n", filename);
      write_file(new_sock);                    // Receive file
      printf("[+]File received and saved as received_from_client.txt\n");

    } else if (strcmp(command, "download") == 0) {
      bzero(filename, 100);                    // Clear filename buffer
      recv(new_sock, filename, 100, 0);        // Receive filename from client
      printf("[*]Client requested file: %s\n", filename);
      send_file(new_sock);                     // Send file to client
      printf("[+]File sent to client.\n");

    } else {
      printf("[-]Unknown command received.\n");
    }

  } else {
    printf("[-]Invalid choice.\n");            // Invalid menu choice
  }

  // Close sockets
  close(new_sock);
  close(sockfd);

  return 0;
}
