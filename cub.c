#include "mlx_linux/mlx.h"
#include "gnl/get_next_line.h"
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PI			3.141592654
#define TWO_PI		6.283185307
#define ONE_DEGREE	0.017453293

int	FPS = 0;

typedef struct		s_dot
{
	double			x;
	double			y;
}					t_dot;

typedef struct		s_frame
{
	void			*ptr;
	char			*addr;
	int				bits_per_pixel;
	int				line_length;
	int				endian;
	int				width;
	int				height;
}					t_frame;

typedef struct		s_keys
{
	int				w;
	int				a;
	int				s;
	int				d;
	int				up;
	int				left;
	int				down;
	int				right;
	int				esc;
}					t_keys;

typedef struct		s_vars
{
	void			*mlx;
	void			*win;
	t_frame			img;
	t_keys			keys;
}					t_vars;

typedef struct		s_list
{
	char			*str;
	struct s_list	*next;
}					t_list;


void	img_init(t_frame *img)
{
	img->addr = (char*)0;
	img->bits_per_pixel = 0;
	img->endian = 0;
	img->height = 0;
	img->ptr = (void*)0;
	img->line_length = 0;
	img->width = 0;
}

void	img_create(void *mlx_ptr, t_frame *img)
{
	img->ptr = mlx_new_image(mlx_ptr, img->width, img->height);
	img->addr = mlx_get_data_addr(img->ptr, &img->bits_per_pixel, &img->line_length, &img->endian);
}

void	keys_init(t_keys *keys)
{
	keys->w = 0;
	keys->a = 0;
	keys->s = 0;
	keys->d = 0;
	keys->up = 0;
	keys->down = 0;
	keys->left = 0;
	keys->right = 0;
	keys->esc = 0;
}

void	vars_init(t_vars *vars, int width, int height, char *title)
{
	vars->mlx = mlx_init();
	img_init(&vars->img);
	keys_init(&vars->keys);
	vars->img.width = width;
	vars->img.height = height;
	vars->win = mlx_new_window(vars->mlx, width, height, title);
}


int		set_keypress(int keycode, t_keys *keys)
{
	if (keycode == 119)
		keys->w = 1;
	else if (keycode == 97)
		keys->a = 1;
	else if (keycode == 115)
		keys->s = 1;
	else if (keycode == 100)
		keys->d = 1;
	else if (keycode == 65362)
		keys->up = 1;
	else if (keycode == 65364)
		keys->down = 1;
	else if (keycode == 65363)
		keys->right = 1;
	else if (keycode == 65361)
		keys->left = 1;
	else if (keycode == 65307)
		keys->esc = 1;
}

int		set_keyrelease(int keycode, t_keys *keys)
{
	if (keycode == 119)
		keys->w = 0;
	else if (keycode == 97)
		keys->a = 0;
	else if (keycode == 115)
		keys->s = 0;
	else if (keycode == 100)
		keys->d = 0;
	else if (keycode == 65362)
		keys->up = 0;
	else if (keycode == 65364)
		keys->down = 0;
	else if (keycode == 65363)
		keys->right = 0;
	else if (keycode == 65361)
		keys->left = 0;
	else if (keycode == 65307)
		keys->esc = 0;
}


double	vec_len(t_dot *start, t_dot end)
{
	end.x -= start->x;
	end.y -= start->y;
	return (sqrt(end.x * end.x + end.y * end.y));
}

double	count_angle_between_dots(t_dot *start, t_dot *end)
{
	return (acos(
					(
						(start->x * end->x) + (start->y * end->y)
					)

					/

					(
						sqrt((start->x * start->x) + (start->y * start->y))

						*

						sqrt((end->x * end->x) + (end->y * end->y))
					)
				)
			);
}

double	rad_to_deg(double angle)
{
	return (angle * 180 / PI);
}

double	deg_to_rad(double angle)
{
	return (angle * PI / 180);
}

void	count_fps()
{
	static int		fps;
	static int		timer;
	const time_t	t = time(NULL);
	struct tm		*time;

	++fps;
	time = localtime(&t);
	if (timer != time->tm_sec)
	{
		timer = time->tm_sec;
		printf("%d\n", fps);
		FPS = fps;
		fps = 0;
	}
	if (!FPS)
		FPS = 100;
}

void	close_win(t_vars *vars)
{
	mlx_loop_end(vars->mlx);
	mlx_destroy_window(vars->mlx, vars->win);
}

void	define_window_center(t_frame *img, t_dot *dot)
{
	dot->x = (double)(img->width / 2);
	dot->y = (double)(img->height / 2);
}

int		compare_args(int n1, int n2)
{
	if (n1 > n2)
		return (n1);
	else
		return (n2);
}


void	put_pixel(t_frame *img, t_dot *dot, int color)
{
	char	*dst;

	if (dot->x > 0 && dot->x < img->width && dot->y > 0 && dot->y < img->height)
	{
		dst = img->addr + ((int)dot->y * img->line_length + (int)dot->x * (img->bits_per_pixel / 8));
		*(unsigned int*)dst = color;
	}
}

void	put_line(t_frame *img, t_dot *dot, int direction, int len, int color)
{
	while (len > 0 && len--)
	{
		put_pixel(img, dot, color);
		if (direction == 'u')
			--(dot->y);
		else if (direction == 'r')
			++(dot->x);
		else if (direction == 'd')
			++(dot->y);
		else if (direction == 'l')
			--(dot->x);
	}
	while (len < 0 && len++)
	{
		put_pixel(img, dot, color);
		if (direction == 'd')
			--(dot->y);
		else if (direction == 'l')
			++(dot->x);
		else if (direction == 'u')
			++(dot->y);
		else if (direction == 'r')
			--(dot->x);
	}
}

void	put_vec(t_frame *img, t_dot start, t_dot end, int color)
{
	int		veclen;

	veclen = (int)vec_len(&start, end);
	while (veclen)
	{
		put_pixel(img, &start, color);
		start.x = ((start.x - end.x) / (double)veclen) * (double)(veclen - 1) + end.x;
		start.y = ((start.y - end.y) / (double)veclen) * (double)(veclen - 1) + end.y;
		--veclen;
	}
}

void	put_vec_len(t_frame *img, t_dot start, t_dot end, int color, int len)
{
	int		veclen;

	veclen = (int)vec_len(&start, end);
	end.x = ((end.x - start.x) / veclen) * (double)len + start.x;
	end.y = ((end.y - start.y) / veclen) * (double)len + start.y;
	while (len)
	{
		put_pixel(img, &start, color);
		start.x = ((start.x - end.x) / (double)len) * (double)(len - 1) + end.x;
		start.y = ((start.y - end.y) / (double)len) * (double)(len - 1) + end.y;
		--len;
	}
}


void	draw_circle(t_frame *img, t_dot *center, int radius, int thickness, int color)
{
	t_dot	edge;
	double	degree;

	degree = 0.0;
	while (thickness--)
	{
		while (degree < TWO_PI)
		{
			edge.x = cos(degree)*(double)radius + center->x;
			edge.y = sin(degree)*(double)radius + center->y;
			put_pixel(img, &edge, color);
			degree += ONE_DEGREE;
		}
		radius--;
		degree = 0.0;
	}
}

void	draw_figure(t_frame *img, t_dot *center, int radius, double rot, int thick, int corner, int color)
{
	t_dot	start;
	t_dot	end;
	double	degree;
	double	delta;
	int		i;

	delta = 360.0 / (double)corner * ONE_DEGREE;
	// if (corner % 2)
	// 	degree = -1.570796327;
	// else
	// 	degree = delta / 2;
	degree = 0.0;
	rot = deg_to_rad(rot);
	while (thick--)
	{
		start.x = cos(degree + rot) * (double)radius + center->x;
		start.y = sin(degree + rot) * (double)radius + center->y;
		i = 0;
		while (i++ < corner)
		{
			degree += delta;
			end.x = cos(degree + rot) * (double)radius + center->x;
			end.y = sin(degree + rot) * (double)radius + center->y;
			put_vec(img, start, end, color);
			start = end;
		}
		radius--;
	}
}


int		main(void)
{
	int		fd;
	char	*str;
	int		cols;
	int		strs;
	fd = open("map.cub", O_RDONLY);

	cols = 0;
	strs = 0;
	while (get_next_line(fd, &str))
	{
		++strs;
		cols = compare_args(cols, slen(str));
	}
	close(fd);


	char	buf[strs][cols + 1];
	int		x;
	int		y;

	fd = open("map.cub", O_RDONLY);
	x = 0;
	while (get_next_line(fd, &str))
	{
		y = 0;
		while (y < slen(str))
		{
			buf[x][y] = str[y];
			++y;
		}
		buf[x][y] = 0 ;
		++x;
	}
	close(fd);

	t_vars	vars;
	t_dot	dot;
	int		i;
	int		j;

	vars_init(&vars, 500, 500, "test window");
	img_create(vars.mlx, &vars.img);

	i = 0;
	while (i < strs)
	{
		j = 0;
		while (j < cols)
		{
			if (buf[i][j] == '1')
			{
				y = i * 10;
				while (y < i * 10 + 10)
				{
					x = j * 10;
					while (x < j * 10 + 10)
					{
						dot.x = x;
						dot.y = y;
						put_pixel(&vars.img, &dot, 0x00ffffff);
						++x;
					}
					++y;
				}
			}
			++j;
		}
		++i;
	}

	mlx_put_image_to_window(vars.mlx, vars.win, vars.img.ptr, 0, 0);
	mlx_loop(vars.mlx);

	// x = 0;
	// y = 0;
	// while (x < strs)
	// {
	// 	printf("%s\n", buf[x++]);
	// }
}
