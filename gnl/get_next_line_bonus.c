/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcinthia <tcinthia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/18 22:16:24 by tcinthia          #+#    #+#             */
/*   Updated: 2021/01/22 19:33:23 by tcinthia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

static int		del_node(t_buf *first, t_buf *file)
{
	t_buf	*front;
	t_buf	*back;

	if (file == first)
		return (0);
	while (first->next != file)
		first = first->next;
	front = first;
	if (file->next)
	{
		back = file->next;
		front->next = back;
		free(file);
	}
	else
	{
		front->next = NULL;
		free(file);
	}
	return (0);
}

static t_buf	*find_node(t_buf *file, int fd)
{
	while (file->next)
	{
		if (file->fd == fd)
			return (file);
		file = file->next;
	}
	if (file->fd == fd)
		return (file);
	return (NULL);
}

static int		start(int fd, char **line, t_buf *file)
{
	t_buf	*new;

	if (BUFFER_SIZE <= 0 || fd < 1 || !line)
		return (0);
	if (!(*line = (char*)malloc(1)))
		return (0);
	**line = 0;
	if (file->fd != fd)
	{
		if (file->fd == 0)
			file->fd = fd;
		else if (find_node(file, fd) == NULL)
		{
			while (file->next)
				file = file->next;
			new = (t_buf*)malloc(sizeof(t_buf));
			new->at = 0;
			new->fd = fd;
			new->next = NULL;
			new->read = 0;
			file->next = new;
		}
	}
	return (1);
}

int				get_next_line(int fd, char **line)
{
	static t_buf	first;
	t_buf			*file;
	size_t			need;
	char			*endl;

	need = 1;
	if (!(start(++fd, line, &first)))
		return (-1);
	file = find_node(&first, fd);
	while (1)
	{
		if (file->at == 0 && need)
		{
			if ((file->read = read(fd - 1, nul(file->buf), BUFFER_SIZE)) < 0)
				return (-1);
			if (file->read == 0)
				return (del_node(&first, file));
			need = 0;
		}
		else if (endl_in_buf(&endl, file))
			return (push(line, file, endl - file->buf, &need));
		else if (push(line, file, slen(file->buf), &need) < 0)
			return (-1);
	}
}
