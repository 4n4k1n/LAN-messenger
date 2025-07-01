# Simple Makefile for Chat Application

CC = gcc
CFLAGS = -Wall -Wextra -pthread
LDFLAGS = -pthread

# Directory structure
SRC_DIR = src
SERVER_DIR = $(SRC_DIR)/server
CLIENT_DIR = $(SRC_DIR)/client

# Source files
SERVER_SOURCES =	$(SERVER_DIR)/server.c \
                	$(SERVER_DIR)/accept_client.c

CLIENT_SOURCES =	$(CLIENT_DIR)/client.c \
                	$(CLIENT_DIR)/parse_msg.c \
                	$(CLIENT_DIR)/receive_msg.c

all: server client

server: $(SERVER_SOURCES)
	$(CC) $(CFLAGS) -o server $(SERVER_SOURCES) $(LDFLAGS)

client: $(CLIENT_SOURCES)
	$(CC) $(CFLAGS) -o client $(CLIENT_SOURCES) $(LDFLAGS)

clean:
	rm -f server client

.PHONY: all clean