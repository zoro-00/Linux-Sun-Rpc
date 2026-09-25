# Quick Start Guide
## Build and Run on macOS — 5 Minutes

### Prerequisites
```bash
# Xcode Command Line Tools (provides gcc, g++, rpcgen)
xcode-select --install
```

### Build
```bash
cd /path/to/eval_1
make
```
Expected output:
```
gcc  -std=gnu11 -w -c fileops_svc.c -o fileops_svc.o
gcc  -std=gnu11 -w -c fileops_xdr.c -o fileops_xdr.o
g++ -std=c++17 -w -c fileops_server.cpp -o fileops_server.o
g++ -std=c++17 -w fileops_server.o fileops_svc.o fileops_xdr.o -o server
gcc  -std=gnu11 -w -c fileops_clnt.c -o fileops_clnt.o
...
g++ -std=c++17 -w fileops_client.o fileops_clnt.o fileops_xdr.o -o client
```

Verify:
```bash
ls -l server client
```

---

## Run Demo (Single Machine)

### Terminal 1 — Start rpcbind (if not already running)
```bash
sudo rpcbind -w
```

### Terminal 2 — Start the Server
```bash
./server
```
The server runs silently until clients connect, logging each RPC call.

### Terminal 3 — Run the Client
```bash
./client localhost
```

---

## Complete Demo Sequence

**Step 1: Create File**
```
Enter choice: 1
Enter filename: demo.txt
Server response [0]: File created successfully
```

**Step 2: Write Content**
```
Enter choice: 2
Enter filename: demo.txt
Enter content: Hello from RPC Client!
Server response [0]: File written successfully
```

**Step 3: Append Content**
```
Enter choice: 3
Enter filename: demo.txt
Enter content to append:  Added via append.
Server response [0]: Content appended successfully
```

**Step 4: Read File**
```
Enter choice: 4
Enter filename: demo.txt
----- File Content -----
Hello from RPC Client! Added via append.
-------------------------
```

**Step 5: Delete File**
```
Enter choice: 5
Enter filename: demo.txt
Server response [0]: File deleted successfully
```

**Step 6: Verify Deletion (proves server-side delete)**
```
Enter choice: 4
Enter filename: demo.txt
Error: Error: file 'demo.txt' not found
```

**Step 7: Exit**
```
Enter choice: 6
Exiting client.
```

---

## Verify Server-Side Logs

Check server terminal output:
```
[SERVER] CREATE_FILE 'demo.txt' -> File created successfully
[SERVER] WRITE_FILE 'demo.txt' -> File written successfully
[SERVER] APPEND_FILE 'demo.txt' -> Content appended successfully
[SERVER] READ_FILE 'demo.txt' (status=0)
[SERVER] DELETE_FILE 'demo.txt' -> File deleted successfully
[SERVER] READ_FILE 'demo.txt' (status=-1)
```

---

## Verify Server Filesystem

```bash
ls -la server_files/
# Empty after demo (file was deleted)

# During demo, while file exists:
cat server_files/demo.txt
# Hello from RPC Client! Added via append.
```

---

## Cleanup

```bash
make clean
# Removes: server, client, *.o, rpcgen outputs, server_files/
```

---

## Remote Machine Setup (Optional)

**On Server Machine**:
```bash
sudo rpcbind -w
./server
```

**On Client Machine**:
```bash
./client <server-IP-address>
# Example:
./client 192.168.1.100
```

Both machines must:
- Be on the same network
- Have port `111` (rpcbind) reachable
- Not have a firewall blocking RPC traffic

---

## Common Errors & Fixes

| Error | Fix |
|-------|-----|
| `Cannot register service` | Run `sudo rpcbind -w` first |
| `Connection refused` | Make sure `./server` is running |
| `clnt_create failed` | Check server IP / rpcbind is running on server |
| `rpcbind: Address already in use` | Kill old process: `pkill rpcbind` |
| `fileops.h: No such file` | Run `rpcgen -a fileops.x` then `make` |

---

## Current File Structure

```
eval_1/
├── fileops.x              ← Protocol definition (you write this)
├── fileops.h              ← rpcgen-generated header
├── fileops_xdr.c          ← rpcgen-generated XDR serialization
├── fileops_clnt.c         ← rpcgen-generated client stub (patched)
├── fileops_svc.c          ← rpcgen-generated server dispatcher (patched)
├── fileops_server.cpp     ← Your server implementation
├── fileops_client.cpp     ← Your client implementation
├── Makefile               ← Build script
├── run_macos.sh           ← Automated run/helper script for macOS
├── server                 ← Compiled server binary
├── client                 ← Compiled client binary
├── server_files/          ← Created at runtime by server
└── *.md                   ← Documentation
```

---

## Architecture Summary

```
┌──────────────────────────────────────────────────────────────┐
│  CLIENT MACHINE                    SERVER MACHINE            │
│                                                              │
│  fileops_client.cpp                fileops_server.cpp        │
│  (user interface, menu)            (actual file operations)  │
│        │                                    │                │
│  fileops_clnt.c  ←── NETWORK ──→  fileops_svc.c             │
│  (client stub)     TCP/RPC        (server dispatcher)        │
│        │                                    │                │
│  fileops_xdr.c ←─────────────────→ fileops_xdr.c            │
│  (serialization)                  (deserialization)          │
│                                                              │
│  Both sides share:  fileops.h  (types & prototypes)          │
└──────────────────────────────────────────────────────────────┘
                         ↑
               rpcbind (port 111)
               maps program# → actual port
```

---

## Tips for Demo to Teacher

✓ **Show the protocol** — Open `fileops.x`, explain the 5 procedures and 3 structs  
✓ **Show the server code** — Explain `FileServer` class in `fileops_server.cpp`  
✓ **Show the client code** — Explain `FileClient` class in `fileops_client.cpp`  
✓ **Run the demo** — Follow the complete sequence above  
✓ **Show server files** — `ls -la server_files/` during/after demo  
✓ **Highlight key points**:
  - RPC makes remote calls look transparent (like local function calls)
  - Client has NO file on its machine — server does ALL I/O
  - rpcbind acts as a name registry (like DNS for services)
  - XDR handles byte-order/encoding differences between machines

That's it — you're ready to demonstrate a complete distributed RPC system! 🚀
