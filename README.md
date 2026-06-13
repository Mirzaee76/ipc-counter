# IPC Counter Application

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg) ![CMake](https://img.shields.io/badge/CMake-3.16%2B-green.svg) ![Linux](https://img.shields.io/badge/Platform-Linux-orange.svg) ![POSIX](https://img.shields.io/badge/POSIX-IPC-red.svg) ![License](https://img.shields.io/badge/License-Educational-lightgrey.svg)

A simple C++17 application that demonstrates inter-process communication (IPC) between two processes using multiple IPC mechanisms:

- Pipes
- UNIX Domain Sockets
- POSIX Shared Memory + Semaphores

The application creates two processes using `fork()`:

- **Initiator (Parent Process)**
- **Receiver (Child Process)**

Both processes exchange a counter value and increment it alternately until a configurable maximum value is reached.

------

# Features

- Multiple IPC implementations behind a common interface
- Factory pattern for IPC channel creation
- POSIX process management (`fork`, `waitpid`)
- Structured logging using `spdlog`
- Command-line configuration
- Easy extension for adding new IPC mechanisms

------

# Project Structure

```text
.
├── CMakeLists.txt
├── include
│   ├── constants.h
│   ├── version.h
│   ├── types.h
│   │
│   ├── core
│   │   ├── application.h
│   │   └── cli_parser.h
│   │
│   └── ipc
│       ├── ipc_channel.h
│       ├── ipc_factory.h
│       ├── pipe_channel.h
│       ├── shm_channel.h
│       └── unix_socket_channel.h
│
├── src
│   ├── main.cpp
│   │
│   ├── core
│   │   ├── application.cpp
│   │   └── cli_parser.cpp
│   │
│   └── ipc
│       ├── ipc_factory.cpp
│       ├── pipe_channel.cpp
│       ├── shm_channel.cpp
│       └── unix_socket_channel.cpp
│
└── third_party
    └── spdlog
```

------

# Architecture

The application is divided into three main layers.

## Core Layer

Responsible for application flow and command-line processing.

### `Application`

Handles:

- Process creation (`fork`)
- Process synchronization
- Message exchange workflow
- IPC channel initialization

### `CliParser`

Parses command-line arguments and fills application configuration options.

------

## IPC Layer

Provides a common abstraction for all IPC implementations.

### `IpcChannel`

Abstract interface implemented by all IPC mechanisms.

```cpp
class IpcChannel
{
public:
    virtual void open(ProcessRole role) = 0;
    virtual void send(const Message& message) = 0;
    virtual Message receive() = 0;
    virtual void close() = 0;

    virtual ~IpcChannel() = default;
};
```

------

### `PipeChannel`

Uses two unnamed POSIX pipes:

```
Parent -----> Child
Parent <----- Child
```

Characteristics:

- Simple
- Fast
- Parent-child only
- Unidirectional per pipe

------

### `UnixSocketChannel`

Uses UNIX Domain Stream Sockets (`AF_UNIX`).

Characteristics:

- Bidirectional
- Local machine only
- Similar API to TCP sockets
- Suitable for unrelated processes

### `SharedMemoryChannel`

Uses:

- POSIX Shared Memory (`shm_open`)
- POSIX Semaphores (`sem_open`)

Semaphores are used to synchronize access to the shared memory region.

Characteristics:

- Highest throughput
- Shared address space
- Explicit synchronization required

------

### `IpcFactory`

Creates the requested IPC implementation at runtime.

```cpp
auto channel = IpcFactory::create(ipcType);
```

This keeps the application independent of the underlying IPC mechanism.

------

# Build Requirements

## Linux

- C++17 compatible compiler
- CMake 3.16+

Tested with:

- GCC

------

# Build Instructions

Clone the repository:

```bash
git clone <repository-url>
cd ipc-counter
```

Create a build directory:

```bash
mkdir build
cd build
```

Configure the project:

```bash
cmake ..
```

Build:

```bash
cmake --build . -j
```

------

# Usage

```bash
./counter_ipc [options]
```

------

## Options

| Option          | Description                |
| --------------- | -------------------------- |
| `--ipc pipe`    | Use Pipe IPC               |
| `--ipc socket`  | Use UNIX Domain Socket IPC |
| `--ipc shm`     | Use Shared Memory IPC      |
| `--max-value N` | Set maximum counter value  |
| `--help`        | Show help                  |
| `--version`     | Show version               |

------

# Examples

## Pipe

```bash
./counter_ipc --ipc pipe
```

------

## UNIX Socket

```bash
./counter_ipc --ipc socket
```

------

## Shared Memory

```bash
./counter_ipc --ipc shm
```

------

## Custom Maximum Value

```bash
./counter_ipc --max-value 20 --ipc shm
```

------

# Example Output

```text
[2026-06-13 18:45:28.823] [70529] [info] application started
[2026-06-13 18:45:28.823] [70529] [info] Initiator started
[2026-06-13 18:45:28.823] [70530] [info] Receiver started
[2026-06-13 18:45:28.823] [70529] [info] Initiator: sent value=0
[2026-06-13 18:45:28.823] [70530] [info] Receiver : recv value=0
[2026-06-13 18:45:28.823] [70530] [info] Receiver : sent value=1
[2026-06-13 18:45:28.823] [70529] [info] Initiator: recv value=1
[2026-06-13 18:45:28.824] [70529] [info] Initiator: sent value=2
[2026-06-13 18:45:28.824] [70530] [info] Receiver : recv value=2
[2026-06-13 18:45:28.824] [70530] [info] Receiver : sent value=3
[2026-06-13 18:45:28.824] [70529] [info] Initiator: recv value=3
[2026-06-13 18:45:28.824] [70530] [info] Receiver finished
[2026-06-13 18:45:28.824] [70529] [info] Initiator finished
[2026-06-13 18:45:28.824] [70529] [info] application finished
```

------

# Extending the Project

Adding a new IPC implementation requires:

1. Creating a new class derived from `IpcChannel`.

```cpp
class MessageQueueChannel : public IpcChannel
{
    ...
};
```

1. Implementing:

```cpp
open(...)
send(...)
receive(...)
close(...)
```

1. Registering it inside `IpcFactory`.

```cpp
case IpcType::MessageQueue:
    return std::make_unique<MessageQueueChannel>();
```

No changes are required in the application logic.

------

# Design Goals

- Demonstrate multiple IPC mechanisms using a unified API.
- Compare different IPC approaches with the same workflow.
- Practice process management and synchronization in Linux.
- Showcase extensible C++ system-programming architecture.

------

# License

This project is provided for educational and learning purposes.
