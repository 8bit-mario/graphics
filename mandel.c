#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include "graphics.h"

#define WIDTH 800
#define HEIGHT 600
#define ITER 50 

void fill_color_palette(int N, Color* a);
float map(float value, float min, float max, float mmin, float mmax);

int main(int argc, char *argv[])
{
	Color palette[ITER];
	fill_color_palette(ITER, palette);
	
	unsigned char* buf = (unsigned char*) malloc(WIDTH*HEIGHT*3);
	ns_init(buf, WIDTH, HEIGHT);

	float zx, zy, cx, cy;
	int x, y, i;
	for(y=0;y<HEIGHT;y++)
	{
		for(x=0;x<WIDTH;x++)
		{
			zx = zy = 0.0f;
			cx = map(x, 0, WIDTH, -2.5f, 1.0f);
			cy = map(y, 0, HEIGHT, -1.5, 1.5f);

			i=0;
			while(zx*zx+zy*zy <= 4.0f && i < ITER-1)
			{
				float tx, ty;
				tx = zx*zx-zy*zy+cx;
				ty = 2.0f * zx*zy+cy;
				zx = tx;
				zy = ty;
				i++;
			}
			/* ToDo: Histogram equalization of iteration */
			ns_set_color(palette[i].r, palette[i].g, palette[i].b);
			ns_plot(x, y);
		}
	}
	write_ppm(buf, "mandel.ppm", WIDTH, HEIGHT);
	free(buf);
	return 0;
}

float map(float value, float min, float max, float mmin, float mmax)
{
	return mmin+(value-min)*(mmax-mmin)/(max- min);
}

void fill_color_palette(int N, Color* a)
{
	/*
	for(int i = 0; i < N; i++)
	{
		a[i].r = map(i, 0, N-1, 1.0f, 0.0f)*255;
		a[i].g = map(i, 0, N-1, 1.0f, 0.0f)*255;
		a[i].b = map(i, 0, N-1, 1.0f, 0.0f)*255;
	}
	*/
	
	for(int i = 0; i < N; i++)
	{
		hsv_to_rgb(45, map(i, 0, N, 1, 0), 1.0, &a[i]);
	}
}
