# 🚀 LAN Messenger

A high-performance, multi-threaded chat application built in C using TCP sockets. Features real-time messaging, colorful terminal output, and robust client-server architecture.

## ✨ Features

- **Real-time messaging** - Instant private messaging between connected clients
- **Multi-threaded architecture** - Handles up to 50 concurrent connections
- **Thread-safe operations** - Mutex-protected client management
- **Unique ID system** - Auto-generated 4-digit IDs for each user
- **Graceful disconnection** - Clean shutdown with proper resource cleanup
- **Debug mode** - Comprehensive logging for development and troubleshooting

## 🛠️ Build & Run

## Join a server
use this command to join a server

if executed type in the server ip address
```bash
bash <(curl -L https://raw.githubusercontent.com/4n4k1n/LAN-messenger/refs/heads/main/setup.sh)
```

### Quick Start
```bash
# Clone the repository
git clone https://github.com/4n4k1n/LAN-messenger.git
cd LAN-messenger

# Build the application
make

# Run the server (in one terminal)
./server

# Connect clients (in separate terminals)
./client
```

### Build Options
```bash
make              # Standard build
make all          # Same as make
make debug        # Build with colorful debug messages
make server       # Build server only
make client       # Build client only
make clean        # Clean build artifacts
make fclean       # Full clean (removes executables)
```

## 🎮 Usage

### Starting the Server
```bash
./server
```
Server starts on `127.0.0.1:12345` and displays status messages.

### Connecting Clients
```bash
./client
```

**Available Commands:**
- `@username#id message` - Send private message
- `/list` - Show online users
- `/quit` - Exit gracefully

### Example Session
```
Enter username: Anakin
Logging in...

=== Personal Messages ===
You are: Anakin#1234

Commands:
  @username#id message  - Send private message
  /list                 - Show online users
  /quit                 - Exit

================================

> /list
Online users:
  Anakin#1234
  Korb#5678
Total: 2 user(s) online

> @Bob#5678 Hello there!
[You → Korb#5678]: Hello there!

> [Korb#5678 → You]: Hi Anakin!
```

## 🏗️ Architecture

### Server Components
- **Main server thread** - Accepts new connections and manages client pool
- **Client handler threads** - One dedicated thread per connected client
- **Thread-safe client management** - Mutex-protected operations for up to 50 clients
- **Message routing system** - Direct messaging and broadcast notifications

### Client Components
- **Main client thread** - Handles user input and connection management
- **Receive thread** - Continuously listens for incoming messages
- **Message parser** - Parses `@username#id message` format

### Network Protocol
```
Login:     LOGIN:username
Private:   PRIVATE:target_name:target_id:message
List:      LIST
Quit:      QUIT

Responses:
WELCOME:username:id
MSG:sender#id:message
SENT:receiver#id:message
LIST:user1#id1,user2#id2,...
ERROR:error_message
NOTIFY:notification
```

## 🔧 Technical Details

### Requirements
- **Compiler**: GCC with C99 support
- **Libraries**: pthread, standard C library
- **Platform**: Linux/Unix-like systems
- **Memory**: Address sanitizer enabled for debugging

### Thread Safety
- Mutex-protected client array operations
- Thread-safe message routing
- Proper resource cleanup on disconnection

### Memory Management
- Dynamic memory allocation for message parsing
- Automatic cleanup on client disconnect
- Address sanitizer for memory leak detection

## 📁 Project Structure

```
LAN-messenger/
├── inc/
│   └── messenger.h          # Header with prototypes and constants
├── src/
│   ├── server/
│   │   ├── server.c         # Main server logic
│   │   └── accept_client.c  # Client connection handling
│   └── client/
│       ├── client.c         # Main client logic
│       ├── receive_msg.c    # Message receiving thread
│       └── parse_msg.c      # Message parsing utilities
├── Dockerfile
├── docker-compose.yml
├── Makefile                 # Build configuration
└── README.md               # This file
```

## 🚀 Development

### Debug Mode
Enable debug mode for detailed logging:
```bash
make debug
./server    # Shows debug messages
./client    # Also shows debug info
```

### Adding Features
1. Follow existing code patterns in `src/`
2. Update function prototypes in `inc/messenger.h`
3. Maintain thread safety with proper mutex usage
4. Test with multiple clients for concurrency issues

## 📜 License

This project is open source and available under the [MIT License](LICENSE).

## 🙏 Acknowledgments

- Built with modern C programming practices
- Thread-safe design patterns
- TCP socket programming fundamentals
