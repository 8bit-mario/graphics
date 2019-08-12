#include<stdlib.h>
#include<stdio.h>
#include<malloc.h>

#define WIDTH 1000
#define HEIGHT 1000
#define ITER 1000000

void write_ppm(unsigned char* buffer, const char* filename, int w, int h);

typedef struct _Point
{
	float x;
	float y;
} Point;

void plot(unsigned char* buffer, int x, int y);

int main(int argc, char *argv[])
{
	int i, v_index;
	float mid_x, mid_y;
	/* allocate image buffer */
	/* so lazy to initialize value, hope all values are zero */
	unsigned char* buffer = (unsigned char*)malloc(WIDTH*HEIGHT*3); 

	Point tri[3] = {{0.0, HEIGHT-1}, {WIDTH-1, HEIGHT-1}, {WIDTH*0.5, 0}};

	Point p = {0.5, 0.5}; /* randomly choosing a point in triangle */
	Point v; /* holds the randomly choosen vertex of triangle */

	/* Algorithm to find the point on sierpinski triangle */
	for(i=0; i<ITER; ++i)
	{
		v_index = rand()%3;
		v = tri[v_index];
		mid_x = (p.x + v.x) * 0.5f; /* compute mid point */
		mid_y = (p.y + v.y) * 0.5f;
		p.x = mid_x;
		p.y = mid_y;
		plot(buffer, p.x, p.y);
	}

	/* write image to ppm format */
	write_ppm(buffer, "sierpinski_triangle.ppm", WIDTH, HEIGHT);
	free(buffer);
	return 0;
}

inline void plot(unsigned char* buffer, int x, int y)
{
	buffer[(x+WIDTH*y)*3+0] = 255; 
	buffer[(x+WIDTH*y)*3+1] = 255;
	buffer[(x+WIDTH*y)*3+2] = 255;
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
	fprintf(fd, "P6 %d %d 255\n", WIDTH, HEIGHT);
	fwrite(buffer, w*h*3, 1, fd);
	fclose(fd);
}
