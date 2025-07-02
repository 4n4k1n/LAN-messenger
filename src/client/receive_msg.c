/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receive_msg.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anakin <anakin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 19:41:13 by anakin            #+#    #+#             */
/*   Updated: 2025/07/02 06:17:16 by anakin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/messenger.h"

static void	receive_type_welcome(char *buffer)
{
	char	*username_start;
	char	*id_start;

	username_start = buffer + 8;
	id_start = strchr(username_start, ':');
	if (id_start)
	{
		*id_start = '\0';
		id_start++;
		strcpy(my_username, username_start);
		strcpy(my_id, id_start);
		printf(COLOR_BOLD_MAGENTA "\n=== Personal Messages ===" COLOR_RESET "\n");
		printf("You are: " COLOR_BOLD_CYAN "%s#%s" COLOR_RESET "\n", my_username, my_id);
		printf(COLOR_BOLD_YELLOW "\nCommands:" COLOR_RESET "\n");
		printf("  " COLOR_CYAN "@username#id message" COLOR_RESET "  - Send private message\n");
		printf("  " COLOR_CYAN "/list" COLOR_RESET "                 - Show online users\n");
		printf("  " COLOR_CYAN "/quit" COLOR_RESET "                 - Exit\n");
		printf(COLOR_BOLD_MAGENTA "\n" "================================" COLOR_RESET "\n");
	}
}

static void	receive_type_msg(char *buffer)
{
	char	*sender;
	char	*message;

	sender = buffer + 4;
	message = strchr(sender, ':');
	if (message)
	{
		*message = '\0';
		message++;
		printf(COLOR_BRIGHT_GREEN "[" COLOR_BOLD_CYAN "%s" COLOR_BRIGHT_GREEN " → You]: " COLOR_WHITE "%s" COLOR_RESET "\n", sender, message);
	}
}

static void	receive_type_sent(char *buffer)
{
	char	*receiver;
	char	*message;

	receiver = buffer + 5;
	message = strchr(receiver, ':');
	if (message)
	{
		*message = '\0';
		message++;
		printf(COLOR_BRIGHT_BLUE "[You → " COLOR_BOLD_CYAN "%s" COLOR_BRIGHT_BLUE "]: " COLOR_WHITE "%s" COLOR_RESET "\n", receiver, message);
	}
}

static void	receive_type_list(char *buffer)
{
	printf(COLOR_BOLD_YELLOW "\nOnline users:" COLOR_RESET "\n");
	char	*users;
	char	*user;
	int		count;

	users = buffer + 5;
	user = strtok(users, ",");
	count = 0;
	
	while (user != NULL)
	{
		printf("  " COLOR_BOLD_CYAN "%s" COLOR_RESET "\n", user);
		count++;
		user = strtok(NULL, ",");
	}
	if (count == 0)
		printf("  " COLOR_GRAY "No users online" COLOR_RESET "\n");
	else
		printf(COLOR_BOLD_GREEN "Total: %d user(s) online" COLOR_RESET "\n", count);
}

void	*receive_msg(void *arg)
{
	(void)arg;
	char	buffer[BUFFER_SIZE];
	int		bytes;

	while (running)
	{
		bytes = recv(socket_fd, buffer, BUFFER_SIZE - 1, 0);
		if (bytes > 0)
		{
			buffer[bytes] = '\0';
			CLEAR_LINE
			if (strncmp(buffer, "WELCOME:", 8) == 0)
				receive_type_welcome(buffer);
			else if (strncmp(buffer, "MSG:", 4) == 0)
				receive_type_msg(buffer);
			else if (strncmp(buffer, "SENT:", 5) == 0)
				receive_type_sent(buffer);
			else if (strncmp(buffer, "LIST:", 5) == 0)
				receive_type_list(buffer);
			else if (strncmp(buffer, "ERROR:", 6) == 0)
				printf(COLOR_BRIGHT_RED "Error: %s" COLOR_RESET "\n", buffer + 6);
			else if (strncmp(buffer, "NOTIFY:", 7) == 0)
				printf(COLOR_BRIGHT_YELLOW "*** %s ***" COLOR_RESET "\n", buffer + 7);
			else
				printf(COLOR_BRIGHT_MAGENTA "Server: %s" COLOR_RESET "\n", buffer);
			write(1, "> ", 2);
			FLUSH
		}
		else if (!bytes)
		{
			printf(COLOR_BRIGHT_RED "\nServer disconnected" COLOR_RESET "\n");
			running = 0;
			break;
		}
		else
		{
			if (running)
			{
				perror("recv failed!");
				running = 0;
			}
			break;
		}
	}
	return (NULL);
}
