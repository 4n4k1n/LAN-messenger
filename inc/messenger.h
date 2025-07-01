/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messenger.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anakin <anakin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 07:20:09 by anakin            #+#    #+#             */
/*   Updated: 2025/07/01 20:08:17 by anakin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSENGER_H
# define MESSENGER_H

# include <stdio.h>         // printf, scanf, etc.
# include <stdlib.h>        // malloc, free, exit
# include <string.h>        // strcpy, strcat, strcmp
# include <unistd.h>        // close, read, write
# include <sys/socket.h>    // socket functions
# include <netinet/in.h>    // sockaddr_in
# include <arpa/inet.h>     // inet_addr
# include <pthread.h>       // threading
# include <time.h>          // time, srand
# include <errno.h>         // error handling
# include "get_next_line.h"

extern int socket_fd;

# define SERVER_PORT 12345
# define SERVER_IP "127.0.0.1"
# define BUFFER_SIZE 512
# define USERNAME_SIZE 50
# define ID_SIZE 10
# define MESSAGE_SIZE 400
# define MAX_CLIENTS 50

// Clear screen
# define CLEAR_SCREEN printf("\033[2J\033[H");

// Move cursor up
# define CURSOR_UP printf("\033[1A");

// Clear current line
# define CLEAR_LINE printf("\033[2K\r");

// Flush output
# define FLUSH fflush(stdout);

# define MAX_MESSGE_LEN 2048

// client globels
extern int 	socket_fd;
extern int	running;
extern char	my_username[USERNAME_SIZE];
extern char	my_id[ID_SIZE];

// server globels
extern pthread_mutex_t clients_mutex;
extern t_client clients[MAX_CLIENTS];
extern int client_count;

// client functions
int	connect_server(void);
int	check_input(char *input, char **hash, char **space);
void	*receive_msg(void *arg);
int	parse_msg(char *input, char *target_name, char *target_id, char *msg);

// server functions
void	generate_id(char *id);
int	send_to_client(int client_index, const char *msg);
void	broadcast_notification(int client_index, const char *msg);
void	*handle_client(void *arg);
void	disconnect_client(int client_index);
int	accept_client(int server_socket);

typedef struct s_client
{
	char		name[USERNAME_SIZE];
	char		id[ID_SIZE];
	char		msg[MESSAGE_SIZE];
	char		formatted_msg[BUFFER_SIZE];
	pthread_t	thread_id;
	int			active;
	int			socket_fd;
}			t_client;

#endif