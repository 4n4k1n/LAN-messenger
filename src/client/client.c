#include "../../inc/messenger.h"

int 	socket_fd;
int		running = 1;
char	my_username[USERNAME_SIZE];
char	my_id[ID_SIZE];

int	connect_server(void)
{
	struct sockaddr_in server_addr;

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
		return (perror("Socket creation failed!"), -1);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
		return (perror("Server connection failed!"), close(socket_fd), -1);
	printf("Connected to server!");
	return (0);
}

static void	cleanup(pthread_t receive_thread)
{
	printf("\nShutting down...\n");
	send(socket_fd, "QUIT", 4, 0);
	pthread_join(receive_thread, NULL);
	close(socket_fd);
}

int	main(void)
{
	char		username[USERNAME_SIZE];
	char		login_msg[USERNAME_SIZE + 10];\
	char		input[BUFFER_SIZE];
	t_client	target;

	if (connect_server() < 0)
		return (1);
	printf("Enter username: ");
	FLUSH
	if (!fgets(username, USERNAME_SIZE, stdin))
		return (printf("failed to read username!"), close(socket_fd), 1);
	username[strcspn(username, "\n")] = '\n';
	if (!username[0])
		return (printf("Username can not be empty!"), close(socket_fd), 1);
	snprintf(login_msg, sizeof(login_msg), "LOGIN:%s", username);
	if (send(socket_fd, login_msg, strlen(login_msg), 0) < 0)
		return (printf("Faild to send username!"), close(socket_fd), 1);
	printf("Logging in...\n");
	if (pthread_create(&target.thread_id, NULL, receive_msg, NULL) != 0)
		return (printf("Failed to create receive thread!"), close(socket_fd), 1);
	while (running)
	{
		write(1, "> ", 2);
		FLUSH
		if (fgets(input, BUFFER_SIZE, stdin))
		{
			input[strcspn(input, "\n")] = '\0';
			if (!input[0])
				continue;
			else if (strcmp(input, "/quit") == 0)
				running = 0;
			else if (strcmp(input, "/list") == 0)
				send(socket_fd, "LIST", 4, 0);
			else if (input[0] == '@')
			{
				if (parse_msg(input, target.name, target.id, target.msg))
				{
					snprintf(target.formatted_msg, sizeof(target.formatted_msg), "PRIVATE:%s:%s:%s", \
						target.name, target.id, target.msg);
					if (send(socket_fd, target.formatted_msg, sizeof(target.formatted_msg), 0) < 0)
						perror("Failed to dend message!");
					else
					{
						printf("Invalid format! Use: @username#id message\n");
						printf("> ");
						FLUSH
					}
				}
			}
			else
			{
				printf("Commands:\n");
				printf("  @username#id message  - Send private message\n");
				printf("  /list                 - Show online users\n");
				printf("  /quit                 - Exit\n");
				printf("> ");
				FLUSH
			}
		}
		else
			running = 0;
	}
	return (cleanup(target.thread_id), 0);
}
