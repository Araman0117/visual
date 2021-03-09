/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcinthia <tcinthia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/18 22:16:24 by tcinthia          #+#    #+#             */
/*   Updated: 2021/01/22 19:33:26 by tcinthia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static int		start(int fd, char **line)
{
	if (BUFFER_SIZE <= 0 || fd < 0 || !line)
		return (0);
	if (!(*line = (char*)malloc(1)))
		return (0);
	**line = 0;
	return (1);
}

static size_t	endl_in_buf(char **endl, t_buf *file)
{
	if ((*endl = sendl(file->buf + file->at, slen(file->buf) - file->at)))
		return (1);
	return (0);
}

static int		push_str(char **line, t_buf *file, size_t len, size_t *need)
{
	if (len - file->at != 0)
		*line = append(*line, file, slen(*line), len);
	file->at = len == slen(file->buf) ? 0 : (len + 1) % slen(file->buf);
	*need = 1;
	return (*line ? 1 : -1);
}

int				get_next_line(int fd, char **line)
{
	static t_buf	file;
	size_t			need;
	char			*endl;

	need = 1;
	if (start(fd, line) == 0)
		return (-1);
	while (1)
	{
		if (file.at == 0 && need == 1)
		{
			if ((file.read = read(fd, nul(file.buf), BUFFER_SIZE)) <= 0)
				return ((file.read < 0) ? -1 : 0);
			need = 0;
		}
		else if (endl_in_buf(&endl, &file))
			return (push_str(line, &file, endl - file.buf, &need));
		else if (push_str(line, &file, slen(file.buf), &need) < 0)
			return (-1);
	}
}
