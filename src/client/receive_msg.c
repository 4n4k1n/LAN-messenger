/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receive_msg.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anakin <anakin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 19:41:13 by anakin            #+#    #+#             */
/*   Updated: 2025/07/01 07:24:35 by anakin           ###   ########.fr       */
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
		printf("\n=== Personal Messages ===\n");
		printf("You are: %s#%s\n", my_username, my_id);
		printf("\nCommands:\n");
		printf("  @username#id message  - Send private message\n");
		printf("  /list                 - Show online users\n");
		printf("  /quit                 - Exit\n");
		printf("\n" "================================" "\n");
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
		printf("[%s → You]: %s\n", sender, message);
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
		printf("[You → %s]: %s\n", receiver, message);
	}
}

static void	receive_type_list(char *buffer)
{
	printf("\nOnline users:\n");
	char	*users;
	char	*user;
	int		count;
	char	my_full_data[USERNAME_SIZE + ID_SIZE + 2];

	users = buffer + 5;
	user = strtok(users, ",");
	count = 0;
	while (users)
	{
		snprintf(my_full_data, sizeof(my_full_data), "%s#%s", my_username, my_id);
		if (strcmp(user, my_username) != 0)
		{
			printf("	%s\n", user);
			count++;
		}
		user = strtok(NULL, ",");
	}
	if (!count)
		printf("No other users online!");
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
				printf("Error: %s\n", buffer + 6);
			else if (strncmp(buffer, "NOTIFY:", 7) == 0)
				printf("*** %s ***\n", buffer + 7);
			else
				printf("Server: %s\n", buffer);
			write(1, "> ", 2);
			FLUSH
		}
		else if (!bytes)
		{
			printf("\nServer disconnected\n");
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
