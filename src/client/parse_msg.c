#include "../../inc/messenger.h"

int	check_input(char *input, char **hash, char **space)
{
	if (input[0] != '@')
		return (1);
	*hash = strchr(input + 1, '#');
	if (!hash)
		return (1);
	*space = strchr(hash + 1, ' ');
	if (!space)
		return (1);
	return (0);
}

static void	extract_data(char *target_data, char *src, char *diff, int max)
{
	int	len;

	len = src - (diff + 1);
	if (len >= max)
		return (1);
	strncpy(target_data, diff + 1, len);
	target_data[len] = '\0';
}

int	parse_msg(char *input, char *target_name, char *target_id, char *msg)
{
	char	*hash;
	char	*space;
	int		len;

	if (check_input(input, &hash, &space) == 1)
		return (1);
	extract_data(target_name, hash, input, USERNAME_SIZE);
	extract_data(target_id, space, hash, ID_SIZE);
	strcpy(msg, space + 1);
	return (0);
}
