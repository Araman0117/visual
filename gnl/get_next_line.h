/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcinthia <tcinthia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/18 22:16:20 by tcinthia          #+#    #+#             */
/*   Updated: 2021/01/21 15:36:38 by tcinthia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stdlib.h>
# include <unistd.h>

# if !defined (BUFFER_SIZE)
#  define BUFFER_SIZE 1024
# endif

typedef struct	s_buf
{
	char		buf[BUFFER_SIZE > 0 ? BUFFER_SIZE + 1 : -BUFFER_SIZE];
	size_t		at;
	ssize_t		read;
}				t_buf;

int				get_next_line(int fd, char **line);
char			*sendl(char *s, size_t len);
size_t			slen(char *str);
char			*nul(char *s);
char			*append(char *line, t_buf *file, size_t oldlen, size_t dist);

#endif
