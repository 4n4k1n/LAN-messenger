# Simple Makefile for Chat Application

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pthread
DEBUG_CFLAGS = -Wall -Wextra -Werror -pthread -DDEBUG
LDFLAGS = -pthread -fsanitize=address,undefined

# Directory structure
SRC_DIR = src
OBJ_DIR = obj
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

debug: server-debug client-debug

server-debug: $(SERVER_SOURCES)
	$(CC) $(DEBUG_CFLAGS) -o server $(SERVER_SOURCES) $(LDFLAGS)

client-debug: $(CLIENT_SOURCES)
	$(CC) $(DEBUG_CFLAGS) -o client $(CLIENT_SOURCES) $(LDFLAGS)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f client server

docker_up:
	docker-compose up -d

docker_client1:
	docker exec -it chat-client1 ./client 172.20.0.10

docker_client2:
	docker exec -it chat-client2 ./client 172.20.0.10

docker_down:
	docker-compose down

re: fclean all

.PHONY: all client server debug server-debug client-debug clean fclean re