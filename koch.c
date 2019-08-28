#include<cairo/cairo.h>
#include<stdio.h>
#include<math.h>
#include<malloc.h>

#define WIDTH 800
#define HEIGHT 600
#define ITER 4 

typedef struct Point
{
	float x, y;
} Point;

void koch(float x0, float y0, float x1, float y1, int N);
void draw_koch(cairo_t* ctx, int N);
void gasket(cairo_t* ctx, float x, float y, float size);

Point* points;
int count = 0;
int main(int argc, char *argv[])
{
	float m, l;
	int c;
	m = sqrtf(3.0f)/2.0f;
	l = 300.0f;
	c = 8*pow(4, ITER-1);
	points = (Point*)malloc(c*sizeof(Point));
	cairo_surface_t* surf;
	cairo_t* ctx;
	surf = cairo_image_surface_create(CAIRO_FORMAT_RGB24, WIDTH, HEIGHT);
	ctx = cairo_create(surf);
	cairo_set_source_rgb(ctx, 1.0, 1.0, 1.0);
	cairo_paint(ctx);
	cairo_save(ctx);
	cairo_translate(ctx, WIDTH/2, HEIGHT/2);
	cairo_set_line_width(ctx, 1.0);
	cairo_set_source_rgb(ctx, 1.0, 0.0, 0.0);
	koch(0.0f, -l, m * l, l/2, ITER);
	draw_koch(ctx, c);
	cairo_stroke(ctx);
	count = 0;
	koch(m * l, l/2, -m * l, l/2, ITER);
	draw_koch(ctx, c);
	cairo_stroke(ctx);
	count = 0;
	koch(-m * l, l/2, 0.0f, -l, ITER);
	draw_koch(ctx, c);
	cairo_stroke(ctx);

	cairo_restore(ctx);
	free(points);
	cairo_surface_write_to_png(surf, "koch.png");
	cairo_surface_destroy(surf);
	cairo_destroy(ctx);
	return 0;
}

void koch(float x0, float y0, float x1, float y1, int N)
{
	float ax, ay, bx, by, cx, cy;
	if(N == 0)
	{
		points[count].x = x0;
		points[count++].y = y0;
		points[count].x = x1;
		points[count++].y = y1;
		return;
	}

	ax = x0+(1.0f/3.0f)*(x1-x0);
	ay = y0+(1.0f/3.0f)*(y1-y0);

	bx = (x0 + x1) * 0.5f + sqrtf(3.0f)*(y1-y0) / 6.0f;
	by = (y0 + y1) * 0.5f - sqrtf(3.0f)*(x1-x0) / 6.0f;

	cx = x0 + (2.0f/3.0f)*(x1-x0);
	cy = y0 + (2.0f/3.0f)*(y1-y0);

	koch(x0, y0, ax, ay, N-1);
	koch(ax, ay, bx, by, N-1);
	koch(bx, by, cx, cy, N-1);
	koch(cx, cy, x1, y1, N-1);
}

void draw_koch(cairo_t* ctx, int N)
{
	for(int i = 0; i < N-1; i+=2)
	{
		cairo_move_to(ctx, points[i].x, points[i].y);
		cairo_line_to(ctx, points[i+1].x, points[i+1].y);
	}
}

void gasket(cairo_t* ctx, float x, float y, float size)
{
	if(size <= 5.0f)
	{
		cairo_rectangle(ctx, x, y, size, size);
		cairo_fill(ctx);
	}
	else
	{
		gasket(ctx, x, y, size/2);
		gasket(ctx, x+size/2, y, size/2);
		gasket(ctx, x+size/2, y+size/2, size/2);
	}
}
