#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SIZE 1024                     // Buffer size for reading/writing
#define MAX_FILE_SIZE 5 * 1024 * 1024 // 5 MB max file size

char filename[100]; // Global variable to hold the file name

// Function to send a file to the server
void send_file(int sockfd) {
  FILE *fp;
  char data[SIZE];
  size_t n;
  size_t total_bytes = 0; // To track the total bytes sent

  fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("[-]Error in reading file.");
    exit(1);
  }

  // Send file contents in chunks
  while ((n = fread(data, 1, SIZE, fp)) > 0) {
    total_bytes += n;

    // Check if file size exceeds 5MB
    if (total_bytes > MAX_FILE_SIZE) {
      printf("[-]File size exceeds 5 MB limit. Aborting.\n");
      fclose(fp);
      return;
    }

    if (send(sockfd, data, n, 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, SIZE); // Clear buffer
  }

  fclose(fp); // Close file after sending
}

// Function to receive a file from server and write to local file
void write_file(int sockfd) {
  int n;
  FILE *fp;
  char buffer[SIZE];
  char *save_as = "received_from_server.txt"; // Output file name
  size_t total_bytes = 0; // Track total bytes received

  fp = fopen(save_as, "w");

  // Receive data until the other side finishes sending
  while ((n = recv(sockfd, buffer, SIZE, 0)) > 0) {
    total_bytes += n;

    // Abort if file is larger than 5MB
    if (total_bytes > MAX_FILE_SIZE) {
      printf("[-]Received file exceeds 5 MB limit. Aborting.\n");
      fclose(fp);
      remove(save_as); // Delete partial file
      return;
    }

    fwrite(buffer, 1, n, fp);
    bzero(buffer, SIZE);

    // If received less than SIZE bytes, assume end of file
    if (n < SIZE)
      break;
  }

  fclose(fp); // Close the file
}

int main() {
  char *ip = "127.0.0.1";             // Server IP address (localhost)
  int port = 8080;                    // Server port
  int sockfd;
  struct sockaddr_in server_addr;    // Struct for server address
  char buffer[SIZE];
  char command[10];                  // Command string: upload/download

  int choice;
  printf("Select mode:\n1 - Listening Mode\n2 - File Transfer Mode\nChoice: ");
  scanf("%d", &choice); // Ask user for mode

  // Create a TCP socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Client socket created successfully.\n");

  // Set server address parameters
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);

  // Connect to server
  if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    perror("[-]Error in connection");
    exit(1);
  }
  printf("[+]Connected to Server.\n");

  if (choice == 2) {
    // File Transfer Mode
    printf("Enter command (upload/download): ");
    scanf("%s", command);
    send(sockfd, command, strlen(command), 0);
    sleep(1); // Small delay to avoid race condition

    if (strcmp(command, "upload") == 0) {
      // Upload file to server
      printf("Enter file name to upload: ");
      scanf("%s", filename);
      send(sockfd, filename, strlen(filename), 0);
      sleep(1);
      send_file(sockfd);
      printf("[+]File uploaded to server.\n");

    } else if (strcmp(command, "download") == 0) {
      // Request file from server
      printf("Enter file name to download: ");
      scanf("%s", filename);
      send(sockfd, filename, strlen(filename), 0);
      sleep(1);
      write_file(sockfd);
      printf("[+]File downloaded from server.\n");

    } else {
      // Invalid command
      printf("[-]Unknown command.\n");
    }

  } else if (choice == 1) {
    // Listening Mode: Wait for command from server
    bzero(buffer, SIZE);
    recv(sockfd, buffer, SIZE, 0);
    strcpy(command, buffer); // Copy command from server

    if (strcmp(command, "upload") == 0) {
      // Server is uploading file
      bzero(filename, 100);
      recv(sockfd, filename, 100, 0);
      printf("[*]Server is uploading file: %s\n", filename);
      write_file(sockfd);
      printf("[+]File received and saved as received_from_server.txt\n");

    } else if (strcmp(command, "download") == 0) {
      // Server requests file
      bzero(filename, 100);
      recv(sockfd, filename, 100, 0);
      printf("[*]Server requested file: %s\n", filename);
      send_file(sockfd);
      printf("[+]File sent to server.\n");
    }

  } else {
    printf("[-]Invalid choice.\n"); // Invalid mode
  }

  close(sockfd); // Close socket connection
  return 0;
}
