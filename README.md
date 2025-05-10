# Simple HTTP Server in C

This project is a basic HTTP server written in pure C. It demonstrates low-level socket programming and basic HTTP request parsing. 
---

## ✨ Features

- Handles simple HTTP `GET` requests.
- Supported routes:
  - `/` – responds with `200 OK`.
  - `/echo/{message}` – echoes back the message in plain text.
  - `/user-agent` – returns the value of the `User-Agent` header.
  - `/files/{filename}` – serves static files from the current directory.
- Basic error handling for non-existent files (returns `400 Not Found`).
- Built using POSIX system calls: `socket()`, `bind()`, `listen()`, `accept()`, `read()`, `write()`, `open()`, etc.

---

## 🛠️ How to Build & Run

1. **Compile the code**:
   ```bash
       make
