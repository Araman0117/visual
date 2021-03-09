/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcinthia <tcinthia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/18 22:16:26 by tcinthia          #+#    #+#             */
/*   Updated: 2021/01/22 19:33:25 by tcinthia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

size_t	slen(char *str)
{
	size_t	len;

	len = 0;
	if (str)
	{
		while (*str++ != 0)
			++len;
	}
	return (len);
}

char	*nul(char *str)
{
	char	*begin_str;
	size_t	len;

	begin_str = str;
	len = BUFFER_SIZE;
	if (len)
		while (len--)
			*str++ = '\0';
	*str = '\0';
	return (begin_str);
}

char	*add(char *line, t_buf *file, size_t oldlen, size_t dist)
{
	char	*src;
	char	*res;
	char	*begin_res;
	char	*begin_line;
	size_t	dstsize;

	dstsize = dist - file->at;
	if (oldlen + dstsize <= oldlen)
		return (line);
	if (!(res = (char*)malloc(oldlen + dstsize + 1)))
	{
		free(line);
		return (NULL);
	}
	begin_res = res;
	begin_line = line;
	while (oldlen--)
		*res++ = *line++;
	free(begin_line);
	src = file->buf + file->at;
	while (dstsize--)
		*res++ = *src++;
	*res = 0;
	return (begin_res);
}

int		push(char **line, t_buf *file, size_t len, size_t *need)
{
	if (len - file->at != 0)
		*line = add(*line, file, slen(*line), len);
	file->at = len == slen(file->buf) ? 0 : (len + 1) % slen(file->buf);
	*need = 1;
	return (*line ? 1 : -1);
}

size_t	endl_in_buf(char **endl, t_buf *file)
{
	char	*str;
	size_t	len;

	str = file->buf + file->at;
	len = slen(file->buf) - file->at;
	while (len--)
		if (*str++ == '\n')
		{
			*endl = str - 1;
			return (1);
		}
	*endl = NULL;
	return (0);
}
