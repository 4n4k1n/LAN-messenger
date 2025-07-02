#include "../../inc/messenger.h"

int 	socket_fd;
int		running = 1;
char	my_username[USERNAME_SIZE];
char	my_id[ID_SIZE];

int	connect_server(const char *server_ip)
{
	struct sockaddr_in server_addr;

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
		return (perror("Socket creation failed!"), -1);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
		return (perror("Server connection failed!"), close(socket_fd), -1);
	printf(COLOR_BRIGHT_GREEN "Connected to server at %s!" COLOR_RESET "\n", server_ip);
	return (0);
}

static void	cleanup(pthread_t receive_thread)
{
	printf(COLOR_BRIGHT_YELLOW "\nShutting down..." COLOR_RESET "\n");
	send(socket_fd, "QUIT", 4, 0);
	pthread_join(receive_thread, NULL);
	close(socket_fd);
}

int	main(int argc, char **argv)
{
	char		username[USERNAME_SIZE];
	char		login_msg[USERNAME_SIZE + 10];
	char		input[BUFFER_SIZE];
	t_client	target;
	pthread_t	receive_thread_id;
	const char	*server_ip;

	server_ip = (argc > 1) ? argv[1] : SERVER_IP;
	if (connect_server(server_ip) < 0)
		return (1);
	printf(COLOR_BOLD_CYAN "Enter username: " COLOR_RESET);
	FLUSH
	if (!fgets(username, USERNAME_SIZE, stdin))
		return (printf(COLOR_BRIGHT_RED "failed to read username!" COLOR_RESET), close(socket_fd), 1);
	username[strcspn(username, "\n")] = '\0';
	if (!username[0])
		return (printf(COLOR_BRIGHT_RED "Username can not be empty!" COLOR_RESET), close(socket_fd), 1);
	snprintf(login_msg, sizeof(login_msg), "LOGIN:%s", username);
	if (send(socket_fd, login_msg, strlen(login_msg), 0) < 0)
		return (printf(COLOR_BRIGHT_RED "Faild to send username!" COLOR_RESET), close(socket_fd), 1);
	printf(COLOR_BRIGHT_BLUE "Logging in..." COLOR_RESET "\n");
	if (pthread_create(&receive_thread_id, NULL, receive_msg, NULL) != 0)
		return (printf(COLOR_BRIGHT_RED "Failed to create receive thread!" COLOR_RESET), close(socket_fd), 1);
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
				if (parse_msg(input, target.name, target.id, target.msg) == 0)
				{
					snprintf(target.formatted_msg, sizeof(target.formatted_msg), "PRIVATE:%s:%s:%s", \
						target.name, target.id, target.msg);
					if (send(socket_fd, target.formatted_msg, strlen(target.formatted_msg), 0) < 0)
						perror("Failed to dend message!");
				}
				else
				{
					printf(COLOR_BRIGHT_RED "Invalid format!" COLOR_RESET " Use: " COLOR_CYAN "@username#id message" COLOR_RESET "\n");
				}
			}
			else
			{
				printf(COLOR_BOLD_YELLOW "Commands:" COLOR_RESET "\n");
				printf("  " COLOR_CYAN "@username#id message" COLOR_RESET "  - Send private message\n");
				printf("  " COLOR_CYAN "/list" COLOR_RESET "                 - Show online users\n");
				printf("  " COLOR_CYAN "/quit" COLOR_RESET "                 - Exit\n");
			}
		}
		else
			running = 0;
	}
	cleanup(receive_thread_id);
	return (0);
}