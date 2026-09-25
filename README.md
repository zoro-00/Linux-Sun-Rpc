# Sun/ONC RPC Remote File Management System

A high-performance, distributed remote file system built with **Sun/ONC RPC (`rpcgen`)** and **C++17**. This application enables remote file operations—including creation, modification, reading, deletion, directory listing, and binary file transfers (uploads/downloads)—transparently across a network over TCP/IP.

---

## 🌟 Key Features

- **Full Remote File Operations**:
  - **Create**: Initialize empty files on the remote server filesystem.
  - **Write**: Overwrite remote files with new content.
  - **Append**: Append data seamlessly to existing remote files.
  - **Read**: Fetch remote file content over RPC.
  - **Delete**: Safely remove files from the server storage directory.
- **Directory Listing**: View all remote files stored on the server along with exact file size metadata.
- **File Transfer Protocol**:
  - **Upload**: Transfer local client files to the remote server filesystem.
  - **Download**: Retrieve files from the server and save them locally.
- **Network Transparency**: Uses Sun RPC XDR (External Data Representation) for network serialization.
- **Modern C++ Architecture**: Object-oriented implementation utilizing C++17 `<filesystem>` and standard streams.

---

## 🏗️ Architecture & Component Overview

```
 +-------------------------+                  +-------------------------+
 |      RPC Client         |                  |       RPC Server        |
 |  (fileops_client.cpp)   |                  |   (fileops_server.cpp)  |
 +------------+------------+                  +------------+------------+
              |                                            |
              v                                            v
     [Client Stub (XDR)]                          [Server Dispatcher]
      (fileops_clnt.c)                              (fileops_svc.c)
              |                                            |
              +==================== TCP/IP ================+
                              Port 111 (rpcbind)
                                       |
                                       v
                                [Server Storage]
                                 (server_files/)
```

### Protocol Definition (`fileops.x`)
- **Program ID**: `0x31000099` | **Version**: `1`
- **Data Structures**:
  - `file_data`: Pair of filename and binary string content buffer (up to 64KB per chunk).
  - `op_result`: Operation status code (`0` success, `-1` failure) and diagnostic response message.
  - `read_result`: Status code and file content buffer.
  - `list_result`: Status code and formatted directory listing string.

---

## 📋 RPC Service Interface Specification

| Proc ID | Procedure Name   | Request Type | Response Type | Description                                   |
|:-------:|:-----------------|:-------------|:--------------|:----------------------------------------------|
| `1`     | `CREATE_FILE`   | `string`     | `op_result`   | Creates a new file on the server.             |
| `2`     | `WRITE_FILE`    | `file_data`  | `op_result`   | Overwrites a file on the server.              |
| `3`     | `APPEND_FILE`   | `file_data`  | `op_result`   | Appends data to an existing file.             |
| `4`     | `READ_FILE`     | `string`     | `read_result` | Reads remote file content.                    |
| `5`     | `DELETE_FILE`   | `string`     | `op_result`   | Deletes a file on the server.                 |
| `6`     | `UPLOAD_FILE`   | `file_data`  | `op_result`   | Uploads a file from client to server.         |
| `7`     | `DOWNLOAD_FILE` | `string`     | `read_result` | Downloads a file from server to client.       |
| `8`     | `LIST_FILES`    | `void`       | `list_result` | Lists all files stored on the server.         |

---

## 🚀 Getting Started

### Prerequisites

#### macOS / Linux
- `gcc` and `g++` (with C++17 support)
- `rpcgen` (Sun RPC protocol compiler)
- `rpcbind` (RPC portmapper daemon)

On macOS, install Xcode Command Line Tools:
```bash
xcode-select --install
```

### 1. Build the Project

Run `make` in the root directory to generate RPC stubs and compile executables:
```bash
make
```

This compiles two primary binaries:
- `server` — The RPC File Server daemon.
- `client` — The interactive menu-driven RPC File Client.

---

## 🎮 Running the Application

### Step 1: Ensure `rpcbind` is running
The `rpcbind` service acts as the RPC portmapper:
```bash
sudo rpcbind -w
```

### Step 2: Start the RPC Server
In terminal 1:
```bash
./server
# Or using helper script:
./run_macos.sh server
```
The server stores managed files under `./server_files/` and logs incoming RPC requests in real time.

### Step 3: Start the RPC Client
In terminal 2 (on the local host or remote network host):
```bash
./client localhost
# Or specify remote server IP:
./client 192.168.1.50
```

---

## 🖥️ Interactive Client Interface

When launched, the client displays an interactive command menu:

```
----- RPC File Operations Menu -----
1. Create File
2. Write File (overwrite)
3. Append to File
4. Read File
5. Delete File
6. List All Files
7. Upload File
8. Download File
9. Exit
```

### Example Usage Session

1. **List Remote Files** (Option `6`):
   ```
   ----- Server Files -----
   1. sample.txt (1024 bytes)
   Total: 1 file(s)
   ------------------------
   ```

2. **Upload a Local File** (Option `7`):
   - Input local path: `./my_document.pdf`
   - Destination remote name: `my_document.pdf`
   - Output: `Server response [0]: File 'my_document.pdf' uploaded successfully (4096 bytes)`

3. **Download a Remote File** (Option `8`):
   - Input remote filename: `sample.txt`
   - Output local save path: `downloaded_sample.txt`
   - Output: `File downloaded successfully to 'downloaded_sample.txt'`

---

## 🧹 Cleanup

To clean binaries, object files, generated stubs, and temporary server files:
```bash
make clean
```

---

## 📁 Repository Structure

```
.
├── fileops.x           # RPC Protocol Definition File
├── fileops_server.cpp  # C++17 Server Engine Implementation
├── fileops_client.cpp  # C++17 Client Application & CLI Interface
├── Makefile            # Build Automation Configuration
├── run_macos.sh        # macOS Startup & Environment Helper
├── README.md           # Project Documentation
└── .gitignore          # Git Exclusion Rules
```
