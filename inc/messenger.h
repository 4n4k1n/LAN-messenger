/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messenger.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anakin <anakin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 07:20:09 by anakin            #+#    #+#             */
/*   Updated: 2025/07/01 20:41:37 by anakin           ###   ########.fr       */
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

// ANSI Color codes
# define COLOR_RESET   "\033[0m"
# define COLOR_RED     "\033[31m"
# define COLOR_GREEN   "\033[32m"
# define COLOR_YELLOW  "\033[33m"
# define COLOR_BLUE    "\033[34m"
# define COLOR_MAGENTA "\033[35m"
# define COLOR_CYAN    "\033[36m"
# define COLOR_WHITE   "\033[37m"
# define COLOR_GRAY    "\033[90m"
# define COLOR_BRIGHT_RED     "\033[91m"
# define COLOR_BRIGHT_GREEN   "\033[92m"
# define COLOR_BRIGHT_YELLOW  "\033[93m"
# define COLOR_BRIGHT_BLUE    "\033[94m"
# define COLOR_BRIGHT_MAGENTA "\033[95m"
# define COLOR_BRIGHT_CYAN    "\033[96m"

// Bold colors
# define COLOR_BOLD_RED     "\033[1;31m"
# define COLOR_BOLD_GREEN   "\033[1;32m"
# define COLOR_BOLD_YELLOW  "\033[1;33m"
# define COLOR_BOLD_BLUE    "\033[1;34m"
# define COLOR_BOLD_MAGENTA "\033[1;35m"
# define COLOR_BOLD_CYAN    "\033[1;36m"

// client globels
extern int 	socket_fd;
extern int	running;
extern char	my_username[USERNAME_SIZE];
extern char	my_id[ID_SIZE];

typedef struct s_client
{
	char		name[USERNAME_SIZE];
	char		id[ID_SIZE];
	char		msg[MESSAGE_SIZE];
	char		formatted_msg[BUFFER_SIZE];
	pthread_t	thread_id;
	int			active;
	int			socket_fd;
	int			index;
}			t_client;

// server globels
extern pthread_mutex_t clients_mutex;
extern t_client clients[MAX_CLIENTS];
extern int client_count;

// client functions
int	connect_server(const char *server_ip);
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

#endif