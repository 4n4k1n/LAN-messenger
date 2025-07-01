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
		printf("[SERVER] Maximum clients reached!\n");
		pthread_mutex_unlock(&clients_mutex);
		close(client_socket);
		return (1);
	}
	return (0);
}

int	wait_for_username(char *buffer, int client_socket)
{
    int bytes;
	
	bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        pthread_mutex_unlock(&clients_mutex);
        close(client_socket);
        return (1);
    }
    buffer[bytes] = '\0';
	return (0);
}

void	init_client(int client_index, int client_socket, char *username)
{
	clients[client_index].socket_fd = client_socket;
	strncpy(clients[client_index].name, username, USERNAME_SIZE - 1);
	clients[client_index].name[USERNAME_SIZE - 1] = '\0';
	generate_id(clients[client_index].id);
	clients[client_index].active = 1;
	pthread_mutex_unlock(&clients_mutex);
}

void	send_welcome_msg(int client_index, struct sockaddr_in client_addr)
{
	char	msg[BUFFER_SIZE];

	snprintf(msg, sizeof(msg), "WELCOME:%s:%s", \
		clients[client_index].name, clients[client_index].id);
	send_to_client(client_index, msg);
	printf("[SERVER] %s#%s connected from %s!\n", \
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
	if (pthread_create(&clients[client_index].thread_id, NULL, handle_client, &client_index) != 0)
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
		return (pthread_mutex_unlock(&clients_mutex), close(client_socket), -1);
	username = buffer + 6;
	init_client(client_index, client_socket, username);
	notify_other_clients(client_index);
	if (create_thread_for_client(client_index) == 1)
		return (-1);
	return (0);
}


