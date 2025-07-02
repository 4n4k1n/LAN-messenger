#include "../../inc/messenger.h"

static int	find_free_client(int client_socket, int *client_index)
{
	*client_index = -1;
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (!clients[i].active)
		{
		*client_index = i;
			if (i >= client_count)
				client_count = i + 1;
			break;
		}
	}
	if (*client_index == -1)
	{
		printf(COLOR_BRIGHT_RED "[SERVER]" COLOR_RESET " Maximum clients reached!\n");
		pthread_mutex_unlock(&clients_mutex);
		close(client_socket);
		return (1);
	}
	return (0);
}

int	wait_for_username(char *buffer, int client_socket)
{
    int bytes;
	
#ifdef DEBUG
	printf(COLOR_CYAN "[DEBUG]" COLOR_RESET " Waiting for username from client...\n");
#endif
	bytes = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes <= 0) {
#ifdef DEBUG
        printf(COLOR_CYAN "[DEBUG]" COLOR_RESET " Failed to receive username, bytes: " COLOR_RED "%d" COLOR_RESET "\n", bytes);
#endif
        pthread_mutex_unlock(&clients_mutex);
        close(client_socket);
        return (1);
    }
    buffer[bytes] = '\0';
#ifdef DEBUG
	printf(COLOR_CYAN "[DEBUG]" COLOR_RESET " Received from client: '" COLOR_YELLOW "%s" COLOR_RESET "' (" COLOR_GREEN "%d" COLOR_RESET " bytes)\n", buffer, bytes);
#endif
	return (0);
}

void	init_client(int client_index, int client_socket, char *username)
{
#ifdef DEBUG
	printf(COLOR_CYAN "[DEBUG]" COLOR_RESET " Initializing client " COLOR_BOLD_MAGENTA "%d" COLOR_RESET "\n", client_index);
#endif
	clients[client_index].socket_fd = client_socket;
	clients[client_index].index = client_index;
	strncpy(clients[client_index].name, username, USERNAME_SIZE - 1);
	clients[client_index].name[USERNAME_SIZE - 1] = '\0';
#ifdef DEBUG
	printf(COLOR_CYAN "[DEBUG]" COLOR_RESET " About to generate ID for client " COLOR_BOLD_MAGENTA "%d" COLOR_RESET "\n", client_index);
#endif
	generate_id(clients[client_index].id);
#ifdef DEBUG
	printf(COLOR_CYAN "[DEBUG]" COLOR_RESET " Generated ID: " COLOR_BRIGHT_GREEN "%s" COLOR_RESET " for client " COLOR_BOLD_MAGENTA "%d" COLOR_RESET "\n", clients[client_index].id, client_index);
#endif
	clients[client_index].active = 1;
#ifdef DEBUG
	printf(COLOR_CYAN "[DEBUG]" COLOR_RESET " Client " COLOR_BOLD_MAGENTA "%d" COLOR_RESET " initialized, unlocking mutex\n", client_index);
#endif
	pthread_mutex_unlock(&clients_mutex);
}

void	send_welcome_msg(int client_index, struct sockaddr_in client_addr)
{
	char	msg[BUFFER_SIZE];

	snprintf(msg, sizeof(msg), "WELCOME:%s:%s", \
		clients[client_index].name, clients[client_index].id);
#ifdef DEBUG
	printf(COLOR_CYAN "[DEBUG]" COLOR_RESET " Welcome message: '" COLOR_BRIGHT_CYAN "%s" COLOR_RESET "'\n", msg);
#endif
	if (send_to_client(client_index, msg) == 0)
	{
#ifdef DEBUG
		printf(COLOR_CYAN "[DEBUG]" COLOR_RESET " Welcome message sent " COLOR_BRIGHT_GREEN "successfully" COLOR_RESET "\n");
#endif
	}
	else
	{
#ifdef DEBUG
		printf(COLOR_CYAN "[DEBUG]" COLOR_RESET " Failed to send welcome message\n");
#endif
	}
	printf(COLOR_BRIGHT_GREEN "[SERVER]" COLOR_RESET " " COLOR_BOLD_CYAN "%s#%s" COLOR_RESET " connected from " COLOR_YELLOW "%s" COLOR_RESET "!\n", \
		clients[client_index].name, clients[client_index].id, \
		inet_ntoa(client_addr.sin_addr));
}

void	notify_other_clients(int client_index)
{
	char	msg[BUFFER_SIZE];

	snprintf(msg, sizeof(msg), "%s#%s joined the room", \
		clients[client_index].name, clients[client_index].id);
	broadcast_notification(client_index, msg);
}

int	create_thread_for_client(int client_index)
{
	if (pthread_create(&clients[client_index].thread_id, NULL, handle_client, &clients[client_index].index) != 0)
	{
		perror("Failed to create client thread!");
		disconnect_client(client_index);
		return (1);
	}
	pthread_detach(clients[client_index].thread_id);
	return (0);
}

int	accept_client(int server_socket)
{
	struct sockaddr_in	client_addr;
	socklen_t			client_len;
	int					client_socket;
	int					client_index;
	char				buffer[BUFFER_SIZE];
	char				*username;

	client_len = sizeof(client_addr);
	client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
	if (client_socket < 0)
		return (perror("Accept failed!"), -1);
	pthread_mutex_lock(&clients_mutex);
	client_index = -1;
	if (find_free_client(client_socket, &client_index) == 1 || wait_for_username(buffer, client_socket) == 1)
		return (-1);
	if (strncmp(buffer, "LOGIN:", 6) != 0)
	{
#ifdef DEBUG
		printf(COLOR_CYAN "[DEBUG]" COLOR_RESET " Invalid login format: '" COLOR_RED "%s" COLOR_RESET "'\n", buffer);
#endif
		return (pthread_mutex_unlock(&clients_mutex), close(client_socket), -1);
	}
	username = buffer + 6;
#ifdef DEBUG
	printf(COLOR_CYAN "[DEBUG]" COLOR_RESET " Login " COLOR_BRIGHT_GREEN "successful" COLOR_RESET ", username: '" COLOR_BOLD_CYAN "%s" COLOR_RESET "'\n", username);
#endif
	init_client(client_index, client_socket, username);
#ifdef DEBUG
	printf(COLOR_CYAN "[DEBUG]" COLOR_RESET " Sending welcome message to client " COLOR_BOLD_MAGENTA "%d" COLOR_RESET "\n", client_index);
#endif
	send_welcome_msg(client_index, client_addr);
	notify_other_clients(client_index);
	if (create_thread_for_client(client_index) == 1)
		return (-1);
	return (0);
}


