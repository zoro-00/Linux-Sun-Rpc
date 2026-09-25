# Quick Start Guide

## Build and Run in 5 Minutes

### Prerequisites
- **GCC / G++ Compiler** (supporting C++17)
- **Sun RPC Tools**: `rpcgen` and `rpcbind`

On macOS, install Xcode Command Line Tools:
```bash
xcode-select --install
```

### Build
```bash
make
```

This compiles `server` and `client` binaries.

---

## Running the Application

### 1. Start `rpcbind`
Ensure the RPC portmapper daemon is running:
```bash
sudo rpcbind -w
```

### 2. Launch the RPC Server
In Terminal 1:
```bash
./server
```
*Alternatively, use the helper script:*
```bash
./run_macos.sh server
```

### 3. Launch the RPC Client
In Terminal 2:
```bash
./client localhost
```
*Or specify a remote server IP address:*
```bash
./client 192.168.1.100
```

---

## Interactive Menu Options

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
