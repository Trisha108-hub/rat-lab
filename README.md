# RAT-Lab

### Educational Client–Server Cybersecurity & Systems Programming Laboratory

![Python](https://img.shields.io/badge/Python-3.x-blue?logo=python)
![C](https://img.shields.io/badge/C-Linux-blue?logo=c)
![TCP](https://img.shields.io/badge/Networking-TCP-orange)
![Platform](https://img.shields.io/badge/Platform-Linux-lightgrey?logo=linux)
![License](https://img.shields.io/badge/License-MIT-green)

> **RAT-Lab is an isolated educational laboratory for studying client-server communication, TCP sockets, file transfer, process interaction, memory mapping, and low-level execution concepts.**

---

## Overview

**RAT-Lab** is a cybersecurity and systems-programming research project designed to explore how a client and server can communicate over a TCP socket and exchange commands, data, and files.

The project contains both **Python** and **C** implementations, allowing the same core concepts to be examined at different abstraction levels.

The laboratory also contains controlled demonstrations of:

- TCP client-server communication
- Command and response handling
- File upload and download
- Process interaction
- Linux memory mapping
- x86-64 machine-code execution concepts
- Runtime memory manipulation
- Low-level C/Python interoperability
- Basic forensic observation of process behavior

The project is intended for **authorized cybersecurity education, malware-analysis research, operating-system studies, and isolated laboratory experimentation**.

---

## Learning Objectives

RAT-Lab was built to explore several important cybersecurity and systems concepts:

### Networking
- TCP socket programming
- Client-server architecture
- Connection handling
- Data serialization
- Network-based command exchange
- File streaming over sockets

### Operating Systems
- Processes and process execution
- File descriptors
- `/proc` interfaces
- Memory mapping
- Process memory concepts
- System calls
- Process behavior observation

### Low-Level Programming
- C socket programming
- Python socket programming
- x86-64 machine code
- Raw byte sequences
- `mmap()`
- `memcpy()`
- Function-pointer execution
- Interaction between native code and Python

### Cybersecurity & Forensics
- Understanding remote-control architectures
- Studying indicators of suspicious process behavior
- Observing file and memory activity
- Understanding how defensive tools can identify unusual execution patterns
- Exploring the relationship between network activity and endpoint behavior

---

## Architecture

At a high level, the laboratory follows a client-server model:

```text
                 ┌─────────────────────┐
                 │      LAB SERVER     │
                 │                     │
                 │  Python / C         │
                 │  TCP Listener       │
                 └──────────┬──────────┘
                            │
                         TCP :5555
                            │
                            ▼
                 ┌─────────────────────┐
                 │      LAB CLIENT     │
                 │                     │
                 │  Python / C         │
                 │  Socket Connection  │
                 └─────────────────────┘
```

The default Python implementation uses a local loopback connection:

```text
127.0.0.1:5555
```

This keeps the default laboratory configuration confined to the local machine.

---

## Repository Structure

```text
rat-lab/
│
├── README.md
├── LICENSE
│
├── server.py
├── backdoor.py
│
├── server.c
├── client.c
│
├── server
├── client
│
├── payload.bin
└── rensics_oof.txt
```

### Key Components

| File | Purpose |
|---|---|
| `server.py` | Python TCP server and command interface |
| `backdoor.py` | Python client-side laboratory component |
| `server.c` | Native C implementation of the server |
| `client.c` | Native C implementation of the client |
| `server` | Compiled native server artifact |
| `client` | Compiled native client artifact |
| `payload.bin` | Binary laboratory artifact |
| `rensics_oof.txt` | Output/artifact associated with the low-level demonstration |

The repository currently contains both Python and native C implementations, making it useful for comparing high-level scripting with lower-level systems programming.

---

## 🔬 Core Laboratory Features

### 1. TCP Socket Communication

The project establishes a TCP communication channel between the laboratory server and client.

The Python implementation uses port `5555` and exchanges serialized command and response data.

The C implementation provides a lower-level equivalent using POSIX socket APIs such as:

- `socket()`
- `bind()`
- `listen()`
- `accept()`
- `connect()`
- `send()`
- `recv()`

This makes the project useful for studying how networking works below the Python abstraction layer.

---

### 2. Command Execution Model

The laboratory demonstrates how a server can send commands to a connected client and receive command output.

This provides a practical example of the relationship between:

```text
Command
   ↓
Network Transport
   ↓
Client Process
   ↓
Operating System
   ↓
Command Output
   ↓
Network Response
```

This behavior is intentionally included as a **controlled research demonstration** rather than as a production remote-access system.

---

### 3. File Transfer

RAT-Lab implements bidirectional file transfer.

```text
Server ────────► Client
       Download

Client ────────► Server
        Upload
```

The implementation transfers files in chunks and tracks the expected file size during transmission. Both the Python and C versions contain dedicated upload/download handling.

This provides a useful exercise in:

- Binary file I/O
- Socket buffering
- Chunked transmission
- File-size tracking
- Network stream handling

---

## Low-Level Memory Laboratory

One of the more advanced parts of RAT-Lab explores **runtime memory behavior and native machine-code execution**.

The C client contains controlled demonstrations involving:

- Anonymous memory mapping
- Executable memory regions
- Copying machine-code bytes into memory
- Function-pointer based execution
- Runtime payload staging

The Python component additionally contains a laboratory demonstration involving `/proc/self/mem` and runtime memory modification.

These components are particularly useful for studying:

```text
Source Code
     ↓
Compiled Machine Code
     ↓
Raw Bytes
     ↓
Memory Mapping
     ↓
Executable Memory
     ↓
CPU Instruction Execution
```

> **Important:** These demonstrations should only be examined inside an isolated research environment.

---

## 🧪 Research & Forensics Perspective

RAT-Lab can also be approached from the **defensive side**.

Instead of focusing only on how the components operate, the laboratory can be used to investigate what traces they generate.

Potential investigation areas include:

### Network Indicators

- TCP connections
- Destination/ source addresses
- Connection timing
- Port activity
- Command-response patterns
- File-transfer traffic

### Process Indicators

- Unexpected child processes
- Suspicious command execution
- Process behavior changes
- Unusual memory mappings
- Executable memory regions

### File-System Indicators

- Newly created files
- Modified files
- Unexpected file transfers
- Binary artifacts

### Memory Indicators

- RWX memory mappings
- Runtime-generated executable regions
- Suspicious memory modification
- Unexpected machine-code execution

This makes the project suitable as a foundation for future **DFIR and malware-analysis exercises**.

---

## 🛠️ Technologies

### Programming Languages

- Python
- C
- x86-64 Assembly / machine code

### Networking

- TCP/IP
- POSIX sockets
- Client-server architecture

### Operating System Concepts

- Linux processes
- File descriptors
- `/proc`
- `mmap`
- Memory permissions
- System calls

### Cybersecurity Concepts

- Remote-control architecture
- Network-based command execution
- File transfer
- Runtime memory analysis
- Binary payload concepts
- Endpoint and network detection

---

## Laboratory Safety

**RAT-Lab is strictly intended for authorized educational and research use.**

Use the project only on systems that you own or have explicit permission to test.

### Recommended environment

```text
┌─────────────────────────────┐
│       Isolated Lab          │
│                             │
│  VM / Dedicated Linux Host  │
│           │                 │
│           ▼                 │
│      RAT-Lab Client         │
│           │                 │
│           ▼                 │
│      RAT-Lab Server         │
│                             │
└─────────────────────────────┘
```

### Recommended precautions

- Use a dedicated virtual machine.
- Prefer a host-only or otherwise isolated network.
- Do not use production systems.
- Do not use real credentials or sensitive files.
- Do not expose the laboratory service to the public Internet.
- Take VM snapshots before experiments.
- Keep experimental artifacts isolated.
- Perform experiments only on systems you are authorized to control.

---

## ⚠️ Security Notice

This repository contains code demonstrating behaviors that are commonly associated with remote-access malware and low-level payload execution.

The purpose of publishing the project is to study these mechanisms from an **educational, systems-programming, cybersecurity, and defensive-analysis perspective**.

The project is **not intended for unauthorized access, persistence, surveillance, credential theft, or deployment against third-party systems**.

If you are conducting security research, ensure that you have appropriate authorization and that the environment is properly isolated.

---

## Suggested Research Extensions

RAT-Lab can be expanded into a much larger cybersecurity research environment.

Possible future work:

- [ ] Encrypted client-server communication
- [ ] Structured command protocol
- [ ] Authentication mechanism
- [ ] Network packet analysis
- [ ] Wireshark-based traffic investigation
- [ ] Process-monitoring experiments
- [ ] Memory-forensics exercises
- [ ] YARA-based artifact detection
- [ ] Syscall tracing
- [ ] MITRE ATT&CK technique mapping
- [ ] IOC generation
- [ ] Detection-rule development
- [ ] SIEM integration
- [ ] Automated forensic logging
- [ ] Defensive dashboard for laboratory telemetry

---

## Educational Use Cases

RAT-Lab can support practical exercises in:

**Operating Systems**

→ Processes, memory, system calls, file descriptors

**Computer Networks**

→ TCP sockets, client-server communication, data streams

**Cybersecurity**

→ Remote-control architectures and attack-surface analysis

**Digital Forensics**

→ Process, file-system, network, and memory artifacts

**Malware Analysis**

→ Behavioral indicators and low-level execution concepts

**Systems Programming**

→ C networking and Linux APIs

**Reverse Engineering**

→ Machine-code representation and runtime behavior

---

## Author

**Trisha108-hub**

Cybersecurity | Digital Forensics | Systems & Security Research

GitHub:  
https://github.com/Trisha108-hub

---

## License

This project is released under the **MIT License**.

See [`LICENSE`](./LICENSE) for details.

---

## Project Status

**Educational / Research Laboratory**

RAT-Lab is an experimental project intended for learning, controlled experimentation, and cybersecurity research.

> **Learn how these mechanisms work. Understand their artifacts. Build better defenses.**
