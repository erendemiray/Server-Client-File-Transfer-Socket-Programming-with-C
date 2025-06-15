# Server-Client File Transfer Socket Programming with C

This is a simple file transfer application using TCP socket programming in C. It allows the **client** to upload a file to the **server**, or download a file from the server.

---

## 🚀 Features

- File upload from client to server
- File download from server to client
- Communication over TCP sockets
- Simple command interface

---

## 🛠️ Technologies Used

- C programming language
- POSIX socket API (Linux/macOS)
- TCP/IP networking

---

## 📁 File Structure

- `server.c`: Server-side source code
- `client.c`: Client-side source code
- `client_file.txt`: Sample file to upload
- `recv.txt`: File received and saved by server
- `downloaded_from_server.txt`: File saved by client after download
- `Makefile`: (optional, you can add one later)

---

## 🧪 How It Works

1. **Start the server**  
   Open a terminal and run:
   ```bash
   gcc server.c -o server
   ./server

2. **Start the client**
Open another terminal and run:  
bash
gcc client.c -o client
./client  

4. **Choose operation**
The client will prompt:

bash
Kopyala
Düzenle
Enter command (upload / download):
Type upload to send client_file.txt to the server (saved as recv.txt)

Type download to receive server_file.txt from the server (saved as downloaded_from_server.txt)

📷 Demo


👨‍💻 Author
Eren Demiray
