#include "../../inc/messenger.h"

t_client clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void	generate_id(char *id)
{
	int	unique;
	int	random_id;

	unique = 0;
	while (!unique)
	{
		random_id = rand() % 9000 + 1000;
		snprintf(id, ID_SIZE, "%d", random_id);
		pthread_mutex_lock(&clients_mutex);
		for (int i = 0; i < client_count; i++)
		{
			if (clients[i].active && strcmp(id, clients[i].id == 0) == 0)
			{
				unique = 0;
				break;
			}
		}
		pthread_mutex_unlock(&clients_mutex);
	}
}

int	send_to_client(int client_index, const char *msg)
{
	if (client_index < 0 || client_index > MAX_CLIENTS || !clients[client_index].active)
		return (-1);
	if (send(clients[client_index].socket_fd, msg, strlen(msg), 0) < 0)
	{
		printf("[SERVER] Failed to send to client %s#%s !\n", \
			clients[client_index].name, clients[client_index].id);
		return (-1);
	}
	return (0);
}

int	find_client(const char *name, const char *id)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (clients[i].active \
			&& strcmp(clients[i].name, name) == 0 \
			&& strcmp(clients[i].id, id) == 0)
			return (i);
	}
	return (-1);
}

void	send_user_list(int client_index)
{
	char	user_list[BUFFER_SIZE];
	int		first;
	char	buffer[USERNAME_SIZE + ID_SIZE + 2];

	strcpy(user_list, "LIST:");
	first = 1;
	pthread_mutex_lock(&clients_mutex);
	for (int i = 0; i < client_count; i++)
	{
		if (clients[i].active)
		{
			if (!first)
				strcat(user_list, ",");
			snprintf(buffer, sizeof(buffer), "%s#%s", \
				clients[client_index].name, clients[client_index].id);
			strcat(user_list, buffer);
			first = 0;
		}
	}
	pthread_mutex_unlock(&clients_mutex);
	send_to_client(client_index, user_list);
}

void	broadcast_notification(int client_index, const char *msg)
{
	char	notification[BUFFER_SIZE];

	snprintf(notification, sizeof(notification), "NOTIFY:%s", msg);
	pthread_mutex_lock(&clients_mutex);
	for (int i = 0; i < client_count; i++)
	{
		if (clients[i].active && i != client_index)
			send_to_client(i, notification);
	}
	pthread_mutex_unlock(&clients_mutex);
}

void	disconnect_client(int client_index)
{
	char	msg[BUFFER_SIZE];

	if (client_index < 0 || client_index > MAX_CLIENTS || !clients[client_index].active)
		return;
	printf("[SERVER] %s#%s disconnected!\n", clients[client_index].name, clients[client_index].id);
	snprintf(msg, BUFFER_SIZE, "%s#%s ledt the room!");
	broadcast_notification(client_index, msg);
	close(clients[client_index].socket_fd);
	clients[client_index].active = 0;
}

void	handle_client_message(int client_index, const char *msg)
{
	char	*msg_cpy;
	char	*target_name;
	char	*target_id;
	char	*content;
	int		target_index;
	char	modified_msg[BUFFER_SIZE];

	printf("[SERVER] Message from %s#%s: %s\n", \
		clients[client_index].name, clients[client_index].id, msg);
	if (strncmp(msg, "PRIVATE:", 8) == 0)
	{
		msg_cpy = strdup(msg + 8);
		target_name = strtok(msg_cpy, ":");
		target_id = strtok(NULL, ":");
		content = strtok(NULL, "");
		if (target_name && target_id && content)
		{
			pthread_mutex_lock(&clients_mutex);
			target_index = find_client(target_name, target_id);
			if (target_index >= 0)
			{
				snprintf(modified_msg, sizeof(modified_msg), "MSG:%s#%s:%s", \
					clients[client_index].name, clients[client_index].id, content);
				if (send_to_client(target_index, content) == 0)
				{
					snprintf(modified_msg, sizeof(modified_msg), "SENT:%s#%s:%s", \
						target_name, target_id, content);
					send_to_client(client_index, modified_msg);
				}
				else
					send_to_client(client_index, "ERROR: Failed to deliver message!");
			}
			else
			{
				snprintf(modified_msg, sizeof(modified_msg), "ERROR: User %s#%s not found!", \
					target_name, target_id);
				send_to_client(client_index, modified_msg);
			}
			pthread_mutex_unlock(&clients_mutex);
		}
		else
			send_to_client(client_index, "ERROR: Invalid message format!");
		free(msg_cpy);
	}
	else if (strcmp(msg, "LIST") == 0)
		send_user_list(client_index);
	else if (strcmp(msg, "QUIT") == 0)
		disconnect_client(client_index);
	else
		send_to_client(client_index, "ERROR: Unknown command!");
}

void	*handle_client(void *arg)
{
	int		client_index;
	char	buffer[BUFFER_SIZE];
	int		bytes;

	while (clients[client_index].active)
	{
		bytes = recv(clients[client_index].socket_fd, buffer, sizeof(buffer) - 1, 0);
		if (bytes > 0)
		{
			buffer[bytes] = '\0';
			handle_client_message(client_index, buffer);
		}
		else if (bytes == 0)
		{
			printf("[SERVER] Client %s#%s disconnected!\n", \
				clients[client_index].name, clients[client_index].id);
				break;
		}
		else
		{
			perror("recv failed!");
			break;
		}
	}
	disconnect_client(client_index);
	return (NULL);
}

int	main(void)
{
	int					server_socket;
	int					opt;
	struct sockaddr_in	server_addr;

	srand(time(NULL));
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0)
		return (perror("Socket creating failed!"), 1);
	opt = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		return (perror("setsockopt failed!"), close(server_socket), 1);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(SERVER_PORT);
	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        return (perror("Bind failed"), close(server_socket), 1);
	if (listen(server_socket, 5) < 0)
		return (perror("listen failed!"), close(server_socket), 1);
	printf("[SERVER] Chat server listening on port %d\n", SERVER_PORT);
    printf("[SERVER] Press Ctrl+C to stop\n");
	while (1)
		accept_client(server_socket);
	close(server_socket);
	return (0);
}
