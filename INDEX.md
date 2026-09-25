# Sun/ONC RPC File Handling System
## System Index & Architecture Guide

---

## Overview

This repository contains a **production-ready RPC (Remote Procedure Call) File System** built using Sun/ONC RPC (`rpcgen`) and C++17. It enables clients to execute file operations (Create, Write, Append, Read, Delete, List, Upload, Download) transparently on a remote server.

**Key Features**:
- ✓ Modern C++ implementation (C++17)
- ✓ Full file management and transfer capabilities (Upload/Download)
- ✓ Interactive menu-driven client interface
- ✓ Real-time directory listing & server storage logging
- ✓ macOS and Linux POSIX compatibility

---

## Quick Navigation

### Getting Started
1. **System Overview & Specs**: `README.md`
2. **Quick Setup & Usage**: `QUICK_START.md`
3. **Build & Run**: Run `make` and `./server` / `./client localhost`

---

## File Structure

```
eval_1/
│
├── Core Source Files
│   ├── fileops.x                    ← RPC protocol definition
│   ├── fileops_server.cpp           ← Server implementation (C++17)
│   ├── fileops_client.cpp           ← Client implementation (C++17)
│   ├── Makefile                     ← Build automation
│   └── run_macos.sh                 ← Helper script for rpcbind & binaries
│
├── Project Documentation
│   ├── README.md                    ← Main project documentation
│   ├── QUICK_START.md              ← Setup & user guide
│   └── INDEX.md                    ← Architecture index
│
└── [Build Outputs]
    ├── fileops.h, fileops_xdr.c, fileops_clnt.c, fileops_svc.c
    ├── server                       ← Server executable
    ├── client                       ← Client executable
    └── server_files/                ← Server storage directory
```

---

## Technical Highlights

### Core Architecture
- **RPC Protocol**: Defined in `fileops.x` (Program `0x31000099`, Version `1`).
- **Data Serialization**: External Data Representation (XDR).
- **Transport**: TCP/IP socket connection handled via `rpcbind` portmapper.
- **Server Engine**: Encapsulated C++17 `FileServer` class interacting with standard file streams.
