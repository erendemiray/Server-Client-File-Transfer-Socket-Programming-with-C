# Client-Server File Transfer in C (Socket Programming)

This project implements a simple TCP-based **client-server file transfer system** using the C programming language. It supports **bidirectional communication**, where both the client and the server can **upload** or **download** files from each other.

---

## ✅ Features

- Built with C using low-level socket APIs
- Supports two-way communication (server ↔ client)
- Upload and download commands from either side
- File transfer using dynamic file names and custom paths
- Maximum file size limit: **5 MB**
- Clear command structure and user interaction
- Graceful handling of large files or failed transfers

---

## 🛠️ How It Works

There are two C files:

- `server.c`: The server-side application
- `client.c`: The client-side application

### Startup

When either side runs the application, it will first prompt the user to **select a mode**:  

Select mode:
1 - Listening Mode
2 - File Transfer Mode
Choice:  

- **Mode 1 (Listening Mode):**
  - Waits for the other side to initiate a command (`upload` or `download`)
- **Mode 2 (File Transfer Mode):**
  - Allows you to actively send a command (`upload` or `download`) to the other side

---

## 📤 File Transfer Commands

After selecting **Mode 2**, the user can type either:

- `upload`: Send a file to the other side
- `download`: Request a file from the other side

The program will then ask for a **file name or full path**, and begin transferring data in chunks.

### Upload

- You provide the filename
- The program checks that the file is under **5 MB**
- The contents are read and sent in blocks (using `send()`)

### Download

- You request a filename
- The remote side sends the file (if found)
- The content is saved as:
  - `received_from_client.txt` (on server)
  - `received_from_server.txt` (on client)

---

## 📁 File Structure

No folders are created or required. The transferred files are always written to the current working directory using fixed output names depending on direction:

- From client → `received_from_client.txt`
- From server → `received_from_server.txt`

---

## ⚠️ Constraints

- Maximum allowed file size: **5 megabytes**
- Exceeding the size results in an error and aborts the transfer
- Only text-based files were tested (binary files may not behave as expected)

---

## ⚙️ Compilation & Execution

### Compile the server:
gcc server.c -o server


##Compile the client:
gcc client.c -o client  

##Run the client:
./client  

🧪 Example Usage
Scenario: Client uploads a file to the server

Server chooses Mode 1 (Listening)

Client chooses Mode 2 (File Transfer)

Client types:

Command: upload

Server receives and saves it as received_from_client.txt

📌 Notes
Sockets are created using socket(AF_INET, SOCK_STREAM, 0)

All file transfers are done using fread, fwrite, send, and recv

Delays (sleep(1)) are added to avoid race conditions on initial message sending

bzero() clears buffers before/after transmission  


👨‍💻 Author
Mustafa Eren Demiray

Final Project – C Programming
Client-Server Socket Programming
