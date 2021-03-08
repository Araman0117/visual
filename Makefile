NAME	= test
CC	= gcc
SRC	= main.c

%.o: %.c
    $(CC) -Wall -Wextra -Werror -I/usr/include -Imlx_linux -O3 -c $< -o $@

all	: $(NAME)

$(NAME): $(OBJ)
    $(CC) -Lmlx_linux -lmlx_linux -L/usr/lib -Imlx_linux -lXext -lX11 -lm -lz -o $(NAME)
