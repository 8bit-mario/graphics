#include<iostream>
#include<cmath>
#include<stdlib.h>
#include"graphics.h"
#define _I(x, y) x + y * 2
#define GX 8
#define GY 8
#define SIZE 256
#define MASK 255

using namespace std;
enum { width = 600, height = 600 };
struct vec2
{
	float x, y;
};

/* processing map function */
float map(float t, float istart, float iend, float ostart, float oend);
/* linear interpolation */
float lerp(float t, float a, float b);
float perlin(float x, float y);
void plot(int x, int y);
void draw_vec(int ix, int iy);
void init();

unsigned char *buff = nullptr;

/* gradient vector storage and permutation table */
float gx[SIZE];
float gy[SIZE];
int perm[SIZE];

int main(int argc, char* argv[])
{
	buff = new unsigned char[width*height*3];

	ns_init(buff, width, height);
	init();

	for(int x = 0; x < width; ++x)
	{
		for(int y = 0; y < height; ++y)
		{
			float fx = map(x, 0, width, 0, GX);
			float fy = map(y, 0, height, 0, GY);
			float p = perlin(fx, fy);
			
			/* input [-1.0, 1.0] to output [0, 255] mapping */
			unsigned char value = map(p, -1.0, 1.0, 0, 255); 

			/* plot color value at pixel (x, y) */
			int index = (x + y * width)*3;
			buff[index++] = value;
			buff[index++] = value;
			buff[index] = value;
		}
	}
	
	/* graident grid visualization */
	ns_set_color(255, 0, 0);
	for(int x = 0; x < GX; x++)
	{
		int px = x * (float) width / GX;
		ns_line(px, 0, px, height);
		ns_line(0, px, width, px);
	}
	/* graident vector visualization */
	ns_set_color(0, 0, 0);
	for(int x = 1; x < GX; x++)
		for(int y = 1; y < GY; y++)
			draw_vec(x, y);
	
	write_ppm(buff, "perlin.ppm", width, height);
	delete[] buff;
	return 0;
}

/* arrow drawing function */
void draw_vec( int ix, int iy)
{
	int h1 = perm[(perm[ix & MASK]+iy)&MASK];
	int px = ix * (float) width / GX;
	int py = iy * (float) height / GY;
	float x = gx[h1];
	float y = gy[h1];
	x *= 0.6 * (float)width / GX;
	y *= 0.6 * (float)height / GY;
	float dx = -x * 0.125 - y * 0.125;
	float dy = -y * 0.125 + x * 0.125;
	float dx1 = -x * 0.125 + y * 0.125;
	float dy1 = -y * 0.125 - x * 0.125;
	ns_line(px, py, px + x, py + y);
	ns_line(px+x, py+y, px+x+dx, py+y+dy);
	ns_line(px+x, py+y, px+x+dx1, py+y+dy1);
}

/* ploting function */
void plot(int x, int y)
{
	int index = (x+y*width)*3;
	buff[index++] = 255;
	buff[index++] = 255;
	buff[index] = 255;
}

float f(float t)
{
	t = fabs(t);
	/* cosine interpolant */
	//return cosf(0.5f * M_PI * t);
	/* hermite interpolant (smoothstep)*/
	return (t >= 1.0) ? 0.0 : 1.0 - 
		(3.0f - 2.0f * t) * t * t;
}

void init()
{
	for(int i = 0; i < SIZE; i++)
	{
		perm[i] = i;
		/* random unit vector from unit circle */
		gx[i] = cosf(2.0f * M_PI * i / SIZE);
		gy[i] = sinf(2.0f * M_PI * i / SIZE);
	}
	/* Fisher-Yates shuffle */
	for(int i = SIZE-1; i > 1; i--)
	{
		int r = rand() % i;
		int t = perm[i];
		perm[i] = perm[r];
		perm[r] = t;
	}
}

/* not great implementation of perlin noise BTW */
float perlin(float x, float y)
{
	int ix0, iy0, ix1, iy1;
	vec2 p1, p2, p3, p4;
	/* compute nearest grid point */
	ix0 = (int)x;
	iy0 = (int)y;
	ix1 = ix0 + 1;
	iy1 = iy0 + 1;

	/* set up gradient from table */
	int h1, h2, h3, h4;
	vec2 v1, v2, v3, v4;
	h1 = perm[(perm[ix0 & MASK] + iy0) & MASK];
	v1.x = gx[h1];
	v1.y = gy[h1];
	h2 = perm[(perm[ix1 & MASK] + iy0) & MASK];
	v2.x = gx[h2];
	v2.y = gy[h2];
	h3 = perm[(perm[ix0 & MASK] + iy1) & MASK];
	v3.x = gx[h3];
	v3.y = gy[h3];
	h4 = perm[(perm[ix1 & MASK] + iy1) & MASK];
	v4.x = gx[h4];
	v4.y = gy[h4];

	/* distance vector of point from integer grid point */
	p1.x = x-(float)ix0;
	p1.y = y-(float)iy0;
	p2.x = x-(float)ix1;
	p2.y = y-(float)iy0;
	p3.x = x-(float)ix0;
	p3.y = y-(float)iy1;
	p4.x = x-(float)ix1;
	p4.y = y-(float)iy1;

	/* dot product of gradient vector and distance vector */
	float d1, d2, d3, d4;
	d1 = p1.x * v1.x + p1.y * v1.y;
	d2 = p2.x * v2.x + p2.y * v2.y;
	d3 = p3.x * v3.x + p3.y * v3.y;
	d4 = p4.x * v4.x + p4.y * v4.y;
	/* smoothing funciton */
	d1 *= f(p1.x)*f(p1.y);
	d2 *= f(p2.x)*f(p2.y);
	d3 *= f(p3.x)*f(p3.y);
	d4 *= f(p4.x)*f(p4.y);

	/* linear interpolation (looks aweful) */
	/*
	float l1, l2, l3;
	float tx, ty;
	tx = x - (float)ix0;
	ty = y - (float)iy0;
	l1 = lerp(tx, d1, d2);
	l2 = lerp(tx, d3, d4);
	l3 = lerp(ty, l1, l2);
	*/
	return d1+d2+d3+d4;
}

float map(float t, float istart, float iend, float ostart, float oend)
{
	return ostart + (oend - ostart) * ((t - istart) / (iend - istart));
}

float lerp(float t, float a, float b)
{
	return a + (b - a) * t;
}
