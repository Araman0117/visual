/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcinthia <tcinthia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/18 22:16:20 by tcinthia          #+#    #+#             */
/*   Updated: 2021/01/22 19:33:24 by tcinthia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# include <stdlib.h>
# include <unistd.h>

# if !defined (BUFFER_SIZE)
#  define BUFFER_SIZE 1024
# endif

typedef struct		s_buf
{
	char			buf[BUFFER_SIZE > 0 ? BUFFER_SIZE + 1 : -BUFFER_SIZE];
	size_t			at;
	ssize_t			read;
	int				fd;
	struct s_buf	*next;
}					t_buf;

int					get_next_line(int fd, char **line);
size_t				slen(char *str);
char				*nul(char *s);
char				*add(char *line, t_buf *file, size_t oldlen, size_t dist);
int					push(char **line, t_buf *file, size_t len, size_t *need);
size_t				endl_in_buf(char **endl, t_buf *file);

#endif
