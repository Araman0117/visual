#include "mlx_linux/mlx.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


typedef struct	s_frame
{
	void		*img;
	char		*addr;
	int			bits_per_pixel;
	int			line_length;
	int			endian;
	int			width;
	int			height;
}				t_frame;

typedef struct	s_vars
{
	void		*mlx;
	void		*win;
	void		*win_param;
	void		*keys;
}				t_vars;

typedef struct	s_win_param
{
	int			width;
	int			height;
}				t_win_param;

typedef struct	s_keys
{
	int			w;
	int			a;
	int			s;
	int			d;
	int			esc;
}				t_keys;


t_frame	*img_init(t_frame *img, t_vars *vars)
{
	t_win_param	*win;

	win = vars->win_param;
	img->img = mlx_new_image(vars->mlx, win->width, win->height);
	img->addr = mlx_get_data_addr(img->img, &(img->bits_per_pixel), &(img->line_length), &(img->endian));
	img->width = win->width;
	img->height = win->height;
	return (img);
}

t_win_param	*win_param_init(int width, int height)
{
	t_win_param	*win;

	win = (t_win_param*)malloc(sizeof(t_win_param));
	win->width = width;
	win->height = height;
	return (win);
}

t_keys	*keys_init(void)
{
	t_keys	*keys;

	keys = (t_keys*)malloc(sizeof(t_keys));
	keys->w = 0;
	keys->a = 0;
	keys->s = 0;
	keys->d = 0;
	keys->esc = 0;
	return (keys);
}

t_vars	*vars_init(t_vars *vars, int width, int height, char *title)
{
	t_win_param	*win;

	vars->mlx = mlx_init();
	vars->win_param = win_param_init(width, height);
	win = vars->win_param;
	vars->win = mlx_new_window(vars->mlx, win->width, win->height, title);
	vars->keys = keys_init();
	return (vars);
}


int		create_color(int r, int g, int b)
{
	return (r << 16 | g << 8 | b);
}

int		continue_rgb(int color)
{
	int	red;
	int	green;
	int	blue;
	int	mask;
	int	offset;

	mask = 0x000000FF;
	red = (color >> 16) & mask;
	green = (color >> 8) & mask;
	blue = color & mask;

	offset = 5;

	if		(red == 255 && green < 255 && !blue)
		green += offset;
	else if	(red && green == 255 && !blue)
		red -= offset;
	else if	(!red && green == 255 && blue < 255)
		blue += offset;
	else if	(!red && green && blue == 255)
		green -= offset;
	else if	(red < 255 && !green && blue == 255)
		red += offset;
	else if	(red == 255 && !green && blue)
		blue -= offset;

	return (red << 16 | green << 8 | blue);
}

double	vec_len(double start_x, double start_y, double end_x, double end_y)
{
	double	diff_x;
	double	diff_y;

	diff_x = end_x - start_x;
	diff_y = end_y - start_y;
	return (sqrt(diff_x * diff_x + diff_y * diff_y));
}

double	count_angle_between_dots(double start_x, double start_y, double end_x, double end_y)
{
	return (acos(
					(
						(start_x * end_x) + (start_y * end_y)
					)

					/

					(
						sqrt((start_x * start_x) + (start_y * start_y))

						*

						sqrt((end_x * end_x) + (end_y * end_y))
					)
				)
			);
}



void	put_pixel(t_frame *img, int x, int y, int color)
{
	char	*dst;

	if (x > 0 && x < img->width && y > 0 && y < img->height)
	{
		dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
		*(unsigned int*)dst = color;
	}
}

void	put_line(t_frame *img, int x, int y, int direction, int len, int color)
{
	if (len > 0)
	{
		while (direction == 'u' && len--)
			put_pixel(img, x, y--, color);
		while (direction == 'r' && len--)
			put_pixel(img, x++, y, color);
		while (direction == 'd' && len--)
			put_pixel(img, x, y++, color);
		while (direction == 'l' && len--)
			put_pixel(img, x--, y, color);
	}
	else
	{
		while (direction == 'd' && len++)
			put_pixel(img, x, y--, color);
		while (direction == 'l' && len++)
			put_pixel(img, x++, y, color);
		while (direction == 'u' && len++)
			put_pixel(img, x, y++, color);
		while (direction == 'r' && len++)
			put_pixel(img, x--, y, color);
	}
}

void	put_vec(t_frame *img, double start_x, double start_y, double end_x, double end_y, int color)
{
	int		veclen;

	veclen = (int)vec_len(start_x, start_y, end_x, end_y);
	while (veclen)
	{
		put_pixel(img, (int)start_x, (int)start_y, color);
		start_x = ((start_x - end_x) / (double)veclen) * (double)(veclen - 1) + end_x;
		start_y = ((start_y - end_y) / (double)veclen) * (double)(veclen - 1) + end_y;
		--veclen;
	}
}

void	put_vec_len(t_frame *img, double start_x, double start_y, double end_x, double end_y, int len, int color)
{
	int		veclen;

	veclen = vec_len(start_x, start_y, end_x, end_y);
	end_x = ((end_x - start_x) / veclen) * (double)len + start_x;
	end_y = ((end_y - start_y) / veclen) * (double)len + start_y;
	while (len)
	{
		put_pixel(img, (int)start_x, (int)start_y, color);
		start_x = ((start_x - end_x) / (double)len) * (double)(len - 1) + end_x;
		start_y = ((start_y - end_y) / (double)len) * (double)(len - 1) + end_y;
		--len;
	}
}

void	put_angle(t_frame *img, t_vars *vars, double start_x, double start_y, double end_x, double end_y, int color)
{
	double		center_x;
	double		center_y;
	int			veclen;
	t_win_param *win;

	win = vars->win_param;
	center_x = win->width / 2;
	center_y = win->height / 2;
	veclen = vec_len(start_x, start_y, end_x, end_y);
	while (veclen)
	{
		put_vec_len(img, center_x, center_y, start_x, start_y, 200, color);
		start_x = ((start_x - end_x) / veclen) * (veclen - 1) + end_x;
		start_y = ((start_y - end_y) / veclen) * (veclen - 1) + end_y;
		--veclen;
	}
}

void	put_trail(t_frame *img, double start_x, double start_y, double end_x, double end_y, int color)
{
	static int	rays;
	static int	particles;
	double		degree;

	degree = 0.017453;

	if (rays++ == 0)
		put_vec(img, start_x, start_y, cos(degree * 200.0) * 10.0 + end_x, sin(degree * 200.0) * 10.0 + end_y, color);
	else if (rays++ == 1)
		put_vec(img, start_x, start_y, cos(degree * 40.0 ) * 10.0 + end_x, sin(degree * 40.0 ) * 10.0 + end_y, color);
	else if (rays++ == 2)
		put_vec(img, start_x, start_y, cos(degree * 120.0) * 10.0 + end_x, sin(degree * 120.0) * 10.0 + end_y, color);
	else if (rays++ == 3)
		put_vec(img, start_x, start_y, cos(degree * 300.0) * 10.0 + end_x, sin(degree * 300.0) * 10.0 + end_y, color);
	else if (rays++ == 4)
		put_vec(img, start_x, start_y, cos(degree * 20.0 ) * 10.0 + end_x, sin(degree * 20.0 ) * 10.0 + end_y, color);
	else if (rays++ == 5)
		put_vec(img, start_x, start_y, cos(degree * 160.0) * 10.0 + end_x, sin(degree * 160.0) * 10.0 + end_y, color);
	else if (rays++ == 6)
		put_vec(img, start_x, start_y, cos(degree * 80.0 ) * 10.0 + end_x, sin(degree * 80.0 ) * 10.0 + end_y, color);
	else
	{
		put_vec(img, start_x, start_y, cos(degree) * 10.0 + end_x, sin(degree) * 10.0 + end_y, color);
		rays = 0;
	}

	if (particles++ == 0)
	{
		put_pixel(img, cos(degree * 85.0 ) * 20.0 + end_x, sin(degree * 85.0 ) * 20.0 + end_y, color);
		put_pixel(img, cos(degree * 21.0 ) * 20.0 + end_x, sin(degree * 21.0 ) * 20.0 + end_y, color);
		put_pixel(img, cos(degree * 244.0) * 20.0 + end_x, sin(degree * 244.0) * 20.0 + end_y, color);
		put_pixel(img, cos(degree * 313.0) * 20.0 + end_x, sin(degree * 313.0) * 20.0 + end_y, color);
	}
	else if (particles++ == 1)
	{
		put_pixel(img, cos(degree * 98.0 ) * 20.0 + end_x, sin(degree * 98.0 ) * 20.0 + end_y, color);
		put_pixel(img, cos(degree * 225.0) * 20.0 + end_x, sin(degree * 225.0) * 20.0 + end_y, color);
		put_pixel(img, cos(degree * 46.0 ) * 20.0 + end_x, sin(degree * 46.0 ) * 20.0 + end_y, color);
		put_pixel(img, cos(degree * 189.0) * 20.0 + end_x, sin(degree * 189.0) * 20.0 + end_y, color);
	}
	else if (particles++ == 2)
	{
		put_pixel(img, cos(degree * 290.0) * 20.0 + end_x, sin(degree * 290.0) * 20.0 + end_y, color);
		put_pixel(img, cos(degree * 140.0) * 20.0 + end_x, sin(degree * 140.0) * 20.0 + end_y, color);
		put_pixel(img, cos(degree * 220.0) * 20.0 + end_x, sin(degree * 220.0) * 20.0 + end_y, color);
		put_pixel(img, cos(degree * 15.0 ) * 20.0 + end_x, sin(degree * 15.0 ) * 20.0 + end_y, color);
	}
	else if (particles++ == 3)
	{
		put_pixel(img, cos(degree * 134.0) * 20.0 + end_x, sin(degree * 134.0) * 20.0 + end_y, color);
		put_pixel(img, cos(degree * 1.0  ) * 20.0 + end_x, sin(degree * 1.0  ) * 20.0 + end_y, color);
		put_pixel(img, cos(degree * 280.0) * 20.0 + end_x, sin(degree * 280.0) * 20.0 + end_y, color);
		put_pixel(img, cos(degree * 340.0) * 20.0 + end_x, sin(degree * 340.0) * 20.0 + end_y, color);
	}
	else
	{
		put_pixel(img, cos(degree * 20.0 ) * 20.0 + end_x, sin(degree * 20.0 ) * 20.0 + end_y, color);
		put_pixel(img, cos(degree * 188.0) * 20.0 + end_x, sin(degree * 188.0) * 20.0 + end_y, color);
		put_pixel(img, cos(degree * 240.0) * 20.0 + end_x, sin(degree * 240.0) * 20.0 + end_y, color);
		put_pixel(img, cos(degree * 12.0 ) * 20.0 + end_x, sin(degree * 12.0 ) * 20.0 + end_y, color);
		particles = 0;
	}
}



void	print_color(t_frame *img, int x, int y, int color)
{
	int	vert;
	int hor;

	vert = 0;
	while (vert < y)
	{
		hor = 0;
		while (hor < x)
		{
			put_pixel(img, hor, vert, color);
			++hor;
		}
		++vert;
	}
}

void	print_rgb	(t_frame *img, int x, int y)
{
	int	color;
	int	vert;
	int	hor;
	int	i;

	vert = 0;
	while (vert < y)
	{
		color = 0x00FF0000;
		i = 0;
		while (i < vert)
		{
			color = continue_rgb(color);
			++i;
		}
		hor = 0;
		while (hor <  x)
		{
			color = continue_rgb(color);
			put_pixel(img, hor, vert, color);
			++hor;
		}
		++vert;
	}
}



void	draw_square(t_frame *img, int center_x, int center_y, int size, int thickness, int color)
{
	int	x;
	int	y;

	while (thickness--)
	{
		x = center_x - size / 2;
		y = center_y - size / 2;
		put_line(img, x, y, 'r', size - 1, color);
		x = x + size - 1;
		put_line(img, x, y, 'd', size - 1, color);
		y = y + size - 1;
		put_line(img, x, y, 'l', size - 1, color);
		x = x - size + 1;
		put_line(img, x, y, 'u', size - 1, color);
		size -= 2;
	}
}

void	draw_circle(t_frame *img, int center_x, int center_y, int radius, int thickness, int color)
{
	int		x;
	int		y;
	double	degree;

	x = 0;
	y = 0;
	degree = 0.0;
	while (thickness--)
	{
		while (degree < 6.2833)
		{
			x = (int)(cos(degree)*(double)radius) + center_x;
			y = (int)(sin(degree)*(double)radius) + center_y;
			put_pixel(img, x, y, color);
			degree += 0.002;
		}
		radius--;
		degree = 0.0;
	}
}

// void	draw_rot_sqr(t_frame *img, int start_x, int start_y, double angle, int size, int thickness, int color)
// {
// 	double	start[2];
// 	double	end[2];
// 	double	center[2];
// 	double	degree;
// 	int		i;
//
// 	thickness *= 2;
// 	degree = 0.7854;
// 	angle = 0.017453 * angle;
// 	center[0] = (double)start_x;
// 	center[1] = (double)start_y;
// 	while (thickness--)
// 	{
// 		start[0] = cos(degree + angle)*sqrt(size*size/2) + center[0];
// 		start[1] = sin(degree + angle)*sqrt(size*size/2) + center[1];
// 		i = 0;
// 		while (i < 4)
// 		{
// 			degree += 1.5708;
// 			end[0] = cos(degree + angle)*sqrt(size*size/2) + center[0];
// 			end[1] = sin(degree + angle)*sqrt(size*size/2) + center[1];
// 			vec_put(img, start, end, color);
// 			start[0] = end[0];
// 			start[1] = end[1];
// 			++i;
// 		}
// 		--size;
// 	}
// }

// void	draw_hexagone(t_frame *img, int start_x, int start_y, int size, int thickness, int color)
// {
// 	double	start[2];
// 	double	end[2];
// 	double	center[2];
// 	double	degree;
// 	int		i;
//
// 	degree = 1.0472;
// 	center[0] = (double)start_x;
// 	center[1] = (double)start_y;
// 	start[0] = (double)size + center[0];
// 	start[1] = center[1];
// 	while (thickness--)
// 	{
// 		i = 0;
// 		while (i < 6)
// 		{
// 			end[0] = (cos(degree)*(double)size) + center[0];
// 			end[1] = (sin(degree)*(double)size) + center[1];
// 			vec_put(img, start, end, color);
// 			start[0] = end[0];
// 			start[1] = end[1];
// 			degree += 1.0472;
// 			++i;
// 		}
// 		size--;
// 	}
// }

void	draw_figure(t_frame *img, int center_x, int center_y, double kx, double ky, int radius, double rot, int thick, int corner, int color)
{
	double	start_x;
	double	start_y;
	double	end_x;
	double	end_y;
	double	degree;
	double	delta;
	int		i;

	delta = 360.0 / (double)corner * 0.017453;
	if (corner % 2)
		degree = -1.570796327;
	else
		degree = delta / 2;
	// degree = 0;
	rot = 0.017453 * rot;
	while (thick--)
	{
		start_x = cos(degree + rot) * (double)radius * kx + (double)center_x;
		start_y = sin(degree + rot) * (double)radius * ky + (double)center_y;
		i = 0;
		while (i < corner)
		{
			degree += delta;
			end_x = cos(degree + rot) * (double)radius * kx + (double)center_x;
			end_y = sin(degree + rot) * (double)radius * ky + (double)center_y;
			put_vec(img, start_x, start_y, end_x, end_y, color);
			start_x = end_x;
			start_y = end_y;
			++i;
		}
		radius--;
	}
}

void	draw_fig_rgb(t_frame *img, int x, int y, double kx, double ky, int radius, double rot, int thick, int corner)
{
	int		color;
	int		vert;
	int		hor;
	int		i;
	char	*dst;

	draw_figure(img, x, y, kx, ky, radius, rot, thick, corner, 0x00FFFFFF);
	vert = 0;
	while (vert < y + radius)
	{
		color = 0x00FF0000;
		i = 0;
		while (i < vert)
		{
			color = continue_rgb(color);
			++i;
		}
		hor = 0;
		while (hor <  x + radius)
		{
			color = continue_rgb(color);
			dst = img->addr + (vert * img->line_length + hor * (img->bits_per_pixel / 8));
			if (*(unsigned int*)dst == 0x00FFFFFF)
				put_pixel(img, hor, vert, color);
			++hor;
		}
		++vert;
	}
}



int		key_hook(int keycode, t_vars *vars)
{
	return (printf("%d\n", keycode));
}

int		rotate_sqr(int keycode, t_vars *vars)
{
	static int		radius = 100;
	static int		thick = 10;
	static int		corner = 5;
	static int		color = 0x003465A4;
	static int		win_width = 800;
	static int		win_height = 600;
	static double	kx = 1.0;
	static double	ky = 1.0;
	static t_frame	img;
	static double	rot = 0.0;
	int				x = win_width/2;
	int				y = win_height/2;

	if (img.img)
		mlx_destroy_image(vars->mlx, img.img);
	if (keycode == 65363)
		rot += 1.0;
	else if (keycode == 65361)
		rot -= 1.0;
	else if (keycode == 65362)
		radius += 1.0;
	else if (keycode == 65364)
		radius -= 1.0;
	else if (keycode == 51)
		corner = 3;
	else if (keycode == 52)
		corner = 4;
	else if (keycode == 53)
		corner = 5;
	else if (keycode == 54)
		corner = 6;
	else if (keycode == 55)
		corner = 7;
	else if (keycode == 56)
		corner = 8;
	else if (keycode == 57)
		corner = 9;
	else if (keycode == 45)
		--corner;
	else if (keycode == 61)
		++corner;
	else
		printf("keycode is %d\n", keycode);
	img.img = mlx_new_image(vars->mlx, win_width, win_height);
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
	draw_figure(&img, x, y, kx, ky, radius, rot, thick, corner, color);
	mlx_put_image_to_window(vars->mlx, vars->win, img.img, 0, 0);
}

int		test_hook(	int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l, int m,
					int n, int o, int p, int q, int r, int s, int t, int u, int v, int w, int x, int y, int z)
{
	printf("test triggered\n");
}

int		mouse_pos(int x, int y)
{
	printf("%d	%d\n", x, y);
}

int		mouse_vec(int keycode, int x, int y, t_vars *vars)
{
	static double	old_x;
	static double	old_y;
	double			angle;
	static t_frame	img;
	t_win_param		*win;

	if (img.img)
		mlx_destroy_image(vars->mlx, img.img);
	// if (!(img.img))
	// {
	// 	img.img = mlx_new_image(vars->mlx, 800, 600);
	// 	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
	// }
	if (old_x && old_y)
	{
		win = vars->win_param;
		angle = count_angle_between_dots(
			old_x - (double)win->width / 2.0,
			old_y - (double)win->height / 2.0,
			(double)(x - win->width / 2),
			(double)(y - win->height / 2));
		img.img = mlx_new_image(vars->mlx, win->width, win->height);
		img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
		put_vec(&img, old_x, old_y, (double)x, (double)y, 0x00FFFFFF);
		mlx_put_image_to_window(vars->mlx, vars->win, img.img, 0, 0);
		printf("%f\n", angle / 0.017453);
	}
	old_x = (double)x;
	old_y = (double)y;
}

int		mouse_angle(int keycode, int x, int y, t_vars *vars)
{
	static double	old_x;
	static double	old_y;
	double			angle;
	static t_frame	img;
	t_win_param		*win;

	if (img.img)
		mlx_destroy_image(vars->mlx, img.img);
	if (old_x && old_y)
	{
		win = vars->win_param;
		img.width = win->width;
		img.height = win->height;
		angle = count_angle_between_dots(
			old_x - (double)win->width / 2.0,
			old_y - (double)win->height / 2.0,
			(double)(x - win->width / 2),
			(double)(y - win->height / 2));
		img.img = mlx_new_image(vars->mlx, win->width, win->height);
		img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
		put_angle(&img, vars, old_x, old_y, (double)x, (double)y, 0x00FFFFFF);
		mlx_put_image_to_window(vars->mlx, vars->win, img.img, 0, 0);
		printf("%f\n", angle / 0.017453);
	}
	old_x = (double)x;
	old_y = (double)y;
}

int		enter_mouse_event_hook(t_vars *vars)
{
	printf("enter hook detected\n");
}

int		leave_mouse_event_hook(t_vars *vars)
{
	printf("leave hook detected\n");
}

int		test_loop_hook(t_vars *vars, int a, int b, int c, int d, int e, int f, int frames)
{
	static t_frame	img;
	static double	rot = 0.0;
	t_win_param		*win;
	int				x;
	int				y;

	win = vars->win_param;
	img.width = win->width;
	img.height = win->height;
	x = win->width/2;
	y = win->height/2;
	rot += 1.0;

	if (img.img)
		mlx_destroy_image(vars->mlx, img.img);
	img.img = mlx_new_image(vars->mlx, win->width, win->height);
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
	draw_figure(&img, x, y, 1.0, 1.0, 100, rot, 10, 5, 0x003465A4);
	mlx_put_image_to_window(vars->mlx, vars->win, img.img, 0, 0);
}

int		set_keypress(int keycode, t_keys *keys)
{
	if		(keycode == 65362)
		keys->w = 1;
	else if	(keycode == 65364)
		keys->s = 1;
	else if	(keycode == 65363)
		keys->d = 1;
	else if	(keycode == 65361)
		keys->a = 1;
	else if	(keycode == 65307)
		keys->esc = 1;
}

int		set_keyrelease(int keycode, t_keys *keys)
{
	if		(keycode == 65362)
		keys->w = 0;
	else if	(keycode == 65364)
		keys->s = 0;
	else if	(keycode == 65363)
		keys->d = 0;
	else if	(keycode == 65361)
		keys->a = 0;
	else if	(keycode == 65307)
		keys->esc = 0;
}

void	define_window_center(double *x, double *y, int *flag, t_win_param *win)
{
	*x = (double)(win->width / 2);
	*y = (double)(win->height / 2);
	*flag = 1;
}

void	show_acc(double delta_w, double delta_a, double delta_s, double delta_d, t_frame *img)
{
	double	len;

	len = 50.0;
	put_line(img, img->width/2, img->height/2 - 5, 'u', (int)(len * delta_w), 0x00ffffff);
	put_line(img, img->width/2 - 5, img->height/2, 'l', (int)(len * delta_a), 0x00ffffff);
	put_line(img, img->width/2, img->height/2 + 5, 'd', (int)(len * delta_s), 0x00ffffff);
	put_line(img, img->width/2 + 5, img->height/2, 'r', (int)(len * delta_d), 0x00ffffff);
}

void	count_acc(double *accel, double *decel, double increm, double decrem, int key)
{
	if (key && *accel < 1.0)
		if (*decel == 0.0)
			*accel += increm;
		else
		{
			*decel -= increm;
			if (*decel < 0.0)
				*decel = 0.0;
		}
	else if (!key && *accel > 0.0)
	{
		*accel -= decrem;
		if (*accel < 0.0)
			*accel = 0.0;
	}
}

void	define_walls(double *var, double *front, double *back, int end, double coeff)
{
	double	temp;

	if (*var < 0.0)
	{
		*var = 0.0;
		temp = *back;
		*back = 0.0;
		*front += temp * coeff;
	}
	else if (*var > (double)end)
	{
		*var = (double)end;
		temp = *front;
		*front = 0.0;
		*back += temp * coeff;
	}
}

void	define_direction(double *x, double *y, double speed, t_keys *keys, t_frame *img)
{
	static double	delta_w = 0;
	static double	delta_a = 0;
	static double	delta_s = 0;
	static double	delta_d = 0;
	static double	delta_x = 0;
	static double	delta_y = 0;
	double			increm;
	double			decrem;
	double			coeff;
	double			temp;

	increm = 0.0004;
	decrem = 0.0002;
	coeff = 0.5;

	count_acc(&delta_w, &delta_s, increm, decrem, keys->w);
	count_acc(&delta_a, &delta_d, increm, decrem, keys->a);
	count_acc(&delta_s, &delta_w, increm, decrem, keys->s);
	count_acc(&delta_d, &delta_a, increm, decrem, keys->d);

	// if (keys->d && delta_x < 1.0)
	// 	delta_x += increm;
	// else if (!keys->d && delta_x > 0.0)
	// 	delta_x -= decrem;
	// if (keys->a && delta_x > -1.0)
	// 	delta_x -= increm;
	// else if (!keys->a && delta_x < 0.0)
	// 	delta_x += decrem;

	// if (keys->s && delta_y < 1.0)
	// 	delta_y += increm;
	// else if (!keys->s && delta_y > 0.0)
	// 	delta_y -= decrem;
	// if (keys->w && delta_x > -1.0)
	// 	delta_y -= increm;
	// else if (!keys->w && delta_y < 0.0)
	// 	delta_y += decrem;

	delta_x = delta_d * speed + delta_a * speed * -1.0;
	delta_y = delta_s * speed + delta_w * speed * -1.0;
	*x += delta_x;
	*y += delta_y;

	put_vec(img, *x, *y, *x - 50 * delta_x, *y - 50 * delta_y, 0x00ffffff);

	define_walls(x, &delta_d, &delta_a, img->width, coeff);
	define_walls(y, &delta_s, &delta_w, img->height, coeff);

	// put_line(img, 150, 150, 'r', (int)(50.0 * delta_x), 0x00ffffff);
	// put_line(img, 150, 150, 'd', (int)(50.0 * delta_y), 0x00ffffff);

	// show_acc(delta_w, delta_a, delta_s, delta_d, img);
}

int		try_exit_game(t_keys *keys, t_vars *vars)
{
	if (keys->esc)
	{
		if (vars->win_param)
			free(vars->win_param);
		if (vars->keys)
			free(vars->keys);
		mlx_loop_end(vars->mlx);
		mlx_destroy_window(vars->mlx, vars->win);
	}
}

void	show_fps(int *frames)
{
	static int		timer;
	const time_t	t = time(NULL);
	struct tm		*time;

	time = localtime(&t);
	if (timer != time->tm_sec)
	{
		timer = time->tm_sec;
		printf("%d\n", *frames);
		*frames = 0;
	}
}

int		game_controlled_fig(t_vars *vars)
{
	t_win_param		*win;
	static int		flag;
	static double	x;
	static double	y;
	static double	rot;
	static t_frame	img;
	static int		frames;

	if (img.img)
		mlx_destroy_image(vars->mlx, img.img);
	img_init(&img, vars);
	win = vars->win_param;
	rot += 1.0;
	if (rot == 360.0)
		rot = 0.0;
	if (!flag)
		define_window_center(&x, &y, &flag, win);
	define_direction(&x, &y, 3.0, vars->keys, &img);
	// draw_figure(&img, img.width/2, img.height/2, 1.0, 1.0, 200, -(rot/3), 5, 3, 0x00406fbf);
	// draw_square(&img, 400, 300, 400, 10, 0x00ff6fbf);
	draw_figure(&img, (int)x, (int)y, 1.0, 1.0, 10, rot, 1, 4, 0x00ffffff);
	// put_pixel(&img, (int)x, (int)y, 0x00FFFFFF);
	mlx_put_image_to_window(vars->mlx, vars->win, img.img, 0, 0);
	++frames;
	show_fps(&frames);
	try_exit_game(vars->keys, vars);
}

void	moving_with_keys(t_vars *vars)
{
	mlx_hook(vars->win, 2, 1L<<0, set_keypress, vars->keys);
	mlx_hook(vars->win, 3, 1L<<1, set_keyrelease, vars->keys);
	mlx_loop_hook(vars->mlx, game_controlled_fig, vars);
	mlx_loop(vars->mlx);
}

int		fps_testing(t_vars *vars)
{
	static int			frames;
	static t_frame		img;
	static t_win_param	*win;
	static double		rot;

	if (img.img)
	{
		mlx_destroy_image(vars->mlx, img.img);
		img.img = mlx_new_image(vars->mlx, win->width, win->height);
	}
	else
	{
		win = vars->win_param;
		img_init(&img, vars);
	}

	rot += 0.1;
	if (rot > 360.0)
		rot = 0.0;

	// put_pixel(&img, 500, 400, 0x00ffffff);
	draw_figure(&img, img.width/2, img.height/2, 1.0, 1.0, 100, rot, 1, 5, 0x00ffffff);

	mlx_put_image_to_window(vars->mlx, vars->win, img.img, 0, 0);
	++frames;
	show_fps(&frames);
	try_exit_game(vars->keys, vars);
}

void	fps(t_vars *vars)
{
	mlx_hook(vars->win, 2, 1L<<0, set_keypress, vars->keys);
	mlx_loop_hook(vars->mlx, fps_testing, vars);
	mlx_loop(vars->mlx);
}


int		main(void)
{
	t_vars	vars;
	// t_frame	img;

	vars_init(&vars, 640, 480, "init test");
	// mlx_do_key_autorepeatoff(vars.mlx);
	// mlx_do_key_autorepeaton(vars.mlx);
	// img_init(&img, &vars);
	// mlx_put_image_to_window(vars.mlx, vars.win, img.img, 0, 0);

	// mlx_hook(vars.win, 2, 1L<<0, rotate_sqr, &vars);
	// mlx_hook(vars.win, 2, 1L<<0, key_hook, &vars);
	// mlx_hook(vars.win, 6, 1L<<6, mouse_pos, &vars);
	// mlx_hook(vars.win, 4, 1L<<2, mouse_angle, &vars);
	// mlx_hook(vars.win, 7, 1L<<4, enter_mouse_event_hook, &vars);
	// mlx_hook(vars.win, 8, 1L<<5, leave_mouse_event_hook, &vars);
	// mlx_hook(vars.win, 4, 1L<<2, test_hook, &vars);
	// mlx_hook(vars.win, , , , &vars);
	// mlx_loop_hook(vars.mlx, test_loop_hook, &vars);
	moving_with_keys(&vars);
	// fps(&vars);

	mlx_loop(vars.mlx);
}

/*
int		set_keypress(int keycode, t_keys *keys)
{
	if		(keycode == 65362)
		keys->w = 1;
	else if	(keycode == 65364)
		keys->s = 1;
	else if	(keycode == 65363)
		keys->d = 1;
	else if	(keycode == 65361)
		keys->a = 1;
	else if	(keycode == 65307)
		keys->esc = 1;
}

int		set_keyrelease(int keycode, t_keys *keys)
{
	if		(keycode == 65362)
		keys->w = 0;
	else if	(keycode == 65364)
		keys->s = 0;
	else if	(keycode == 65363)
		keys->d = 0;
	else if	(keycode == 65361)
		keys->a = 0;
	else if	(keycode == 65307)
		keys->esc = 0;
}

void	define_window_center(double *x, double *y, int *flag, t_win_param *win)
{
	*x = (double)(win->width / 2);
	*y = (double)(win->height / 2);
	*flag = 1;
}

void	show_acc(double *x, double *y, double delta_w, double delta_a, double delta_s, double delta_d, t_frame *img)
{
	double	len;

	len = 50.0;
	put_line(img, (int)*x, (int)(*y - 6), 'u', (int)(len * delta_s), 0x00ffffff);
	put_line(img, (int)(*x - 6), (int)*y, 'l', (int)(len * delta_d), 0x00ffffff);
	put_line(img, (int)*x, (int)(*y + 6), 'd', (int)(len * delta_w), 0x00ffffff);
	put_line(img, (int)(*x + 6), (int)*y, 'r', (int)(len * delta_a), 0x00ffffff);
}

void	count_acc(double *accel, double *decel, double increm, double decrem, int key)
{
	if (key && *accel < 1.0)
		if (*decel == 0.0)
			*accel += increm;
		else
		{
			*decel -= increm;
			if (*decel < 0.0)
				*decel = 0.0;
		}
	else if (!key && *accel > 0.0)
	{
		*accel -= decrem;
		if (*accel < 0.0)
			*accel = 0.0;
	}
}

void	define_walls(double *var, double *front, double *back, int end, double coeff)
{
	double	temp;

	if (*var < 0.0)
	{
		*var = 0.0;
		temp = *back;
		*back = 0.0;
		*front += temp * coeff;
	}
	else if (*var > (double)end)
	{
		*var = (double)end;
		temp = *front;
		*front = 0.0;
		*back += temp * coeff;
	}
}

void	define_direction(double *x, double *y, double speed, t_keys *keys, t_frame *img)
{
	static double	delta_w = 0;
	static double	delta_a = 0;
	static double	delta_s = 0;
	static double	delta_d = 0;
	double			increm;
	double			decrem;
	double			coeff;
	double			temp;

	increm = 0.002;
	decrem = 0.002;
	coeff = 0.5;

	count_acc(&delta_w, &delta_s, increm, decrem, keys->w);
	count_acc(&delta_a, &delta_d, increm, decrem, keys->a);
	count_acc(&delta_s, &delta_w, increm, decrem, keys->s);
	count_acc(&delta_d, &delta_a, increm, decrem, keys->d);

	*x += delta_d * speed + delta_a * speed * -1.0;
	define_walls(x, &delta_d, &delta_a, img->width, coeff);

	*y += delta_s * speed + delta_w * speed * -1.0;
	define_walls(y, &delta_s, &delta_w, img->height, coeff);

	show_acc(x, y, delta_w, delta_a, delta_s, delta_d, img);
}

int		try_exit_game(t_keys *keys, t_vars *vars)
{
	if (keys->esc)
	{
		if (vars->win_param)
			free(vars->win_param);
		if (vars->keys)
			free(vars->keys);
		mlx_loop_end(vars->mlx);
		mlx_destroy_window(vars->mlx, vars->win);
	}
}

int		game_controlled_fig(t_vars *vars)
{
	t_win_param		*win;
	static int		flag;
	static double	x;
	static double	y;
	static double	rot;
	static t_frame	img;

	if (img.img)
		mlx_destroy_image(vars->mlx, img.img);
	img_init(&img, vars);
	win = vars->win_param;
	rot += 1.0;
	if (rot == 360.0)
		rot = 0.0;
	if (!flag)
		define_window_center(&x, &y, &flag, win);
	define_direction(&x, &y, 3.0, vars->keys, &img);
	// draw_figure(&img, 400, 300, 1.0, 1.0, 200, -rot, 5, 3, 0x00406fbf);
	// draw_square(&img, 400, 300, 400, 10, 0x00406fbf);
	draw_figure(&img, (int)x, (int)y, 1.0, 1.0, 10, rot, 4, 5, 0x00ffffff);
	mlx_put_image_to_window(vars->mlx, vars->win, img.img, 0, 0);
	try_exit_game(vars->keys, vars);
}

void	moving_with_keys(t_vars *vars)
{
	mlx_hook(vars->win, 2, 1L<<0, set_keypress, vars->keys);
	mlx_hook(vars->win, 3, 1L<<1, set_keyrelease, vars->keys);
	mlx_loop_hook(vars->mlx, game_controlled_fig, vars);
}
*/
