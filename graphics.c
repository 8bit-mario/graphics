#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "graphics.h"

typedef struct _Color
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} Color;

unsigned char *_ns_buffer;
int _ns_buffer_width;
int _ns_buffer_height;
Color _ns_color;

void ns_clear()
{
	memset(_ns_buffer, 0, _ns_buffer_width*_ns_buffer_height*3);
}

void ns_set_color(unsigned char r, unsigned char g, unsigned char b)
{
	_ns_color.r = r;
	_ns_color.g = g;
	_ns_color.b = b;
}

void ns_init(unsigned char* buffer, int width, int height)
{
	_ns_buffer = buffer;

	_ns_buffer_width = width;
	_ns_buffer_height = height;

	_ns_color.r = 255;
	_ns_color.b = 255;
	_ns_color.g = 255;
}

void ns_line(int x1, int y1, int x2, int y2)
{	
	int dx, dy, step, i;
	float xInc, yInc, x, y;
	dx = x2 - x1; dy = y2 - y1;
	x = x1; y = y1;
	if(abs(dx) > abs(dy))
		step = abs(dx);
	else
		step = abs(dy);
	xInc = dx / (float) step;
	yInc = dy / (float) step;
	for(i=0;i<step;++i)
	{
		x += xInc;
		y += yInc;
		ns_plot((int)(x+0.5), (int)(y+0.5));
	}
}

void ns_rect(int x, int y, int w, int h)
{
	for(int i = 0; i < w; i++)
	{
		ns_plot(x+i, y);
		ns_plot(x+i, y+h);
	}
	for(int i = 0; i < h; i++)
	{
		ns_plot(x, y+i);
		ns_plot(x+w, y+i);
	}
}

inline void ns_plot(int x, int y)
{
	int index = (x+_ns_buffer_width*y)*3;
	_ns_buffer[index++] = _ns_color.r;
	_ns_buffer[index++] = _ns_color.g;
	_ns_buffer[index] = _ns_color.b;
}

void write_ppm(unsigned char* buffer, const char* filename, int w, int h)
{
	FILE* fd;
	fd = fopen(filename, "wb");
	if(fd == NULL)
	{
		printf("Error opening file: %s\n", filename);
		exit(1);
	}
	fprintf(fd, "P6 %d %d 255\n", w, h);
	fwrite(buffer, w*h*3, 1, fd);
	fclose(fd);
}
