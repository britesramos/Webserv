# Webserv

Webserv is a lightweight HTTP web server written entirely in **C++**, without external libraries, developed as part of a Codam project in collaboration with @edooarda.  
It aims to implement core HTTP 1.1 functionality, handle multiple clients concurrently, and serve static as well as dynamic content through CGI.

---

## 🚀 Features

- Non-blocking HTTP server using `epoll()`
- Supports:
  - **GET**, **POST**, and **DELETE** methods
  - Serving static files
  - File uploads
  - Default error pages
  - Multiple servers and ports
  - Directory listing
  - HTTP redirects
  - CGI execution (e.g. PHP, Python)
- Configuration file with:
  - Hosts, ports, and server names
  - Custom routes and methods
  - Max body size limits
  - Default index pages
- Compatible with standard web browsers

---

## 📷 Screenshots

*(Add screenshots or terminal output if you have any. E.g.:)*

![Webserv in action](./screenshots/server_running.png)

---

## ⚙️ Installation

### Prerequisites

- C++ compiler
- Unix-based OS (Linux, macOS)

### Build

Clone the repository + Compile and run the code:

```bash
git clone https://github.com/britesramos/webserv.git
cd webserv
make
./webserv
