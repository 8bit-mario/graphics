#include<stdio.h>
#include<math.h>
#include<cairo/cairo.h>

typedef struct Point
{
	float x;
	float y;
} Point;

Point bezier(Point* p1, Point* p2, Point* p3, float t);
int main(int argc, char *argv[])
{
	cairo_t *ctx;
	cairo_surface_t *surface;
	float t;
	Point p1 = {40.0, 100.0}, p2 = {400.0, 400.0}, p3 = {600.0, 190.0};
	Point p;
	double dash_pattern = 8.0;

	surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 800, 600);
	ctx = cairo_create(surface);
	/* Draw Control Points */
	cairo_set_source_rgb(ctx, 1.0, 1.0, 1.0);
	cairo_paint(ctx);
	cairo_set_source_rgb(ctx, 0.0, 1.0, 0.0);
	cairo_arc(ctx, p1.x, p1.y, 10, 0, 2*M_PI);
	cairo_fill(ctx);
	cairo_arc(ctx, p2.x, p2.y, 10, 0, 2*M_PI);
	cairo_fill(ctx);
	cairo_arc(ctx, p3.x, p3.y, 10, 0, 2*M_PI);
	cairo_fill(ctx);

	cairo_set_source_rgb(ctx, 1.0, 0.0, 0.0);
	cairo_move_to(ctx, p1.x, p1.y);
	for(t=0.0; t<=1.0; t+=0.01)
	{
		p = bezier(&p1, &p2, &p3, t);
		cairo_line_to(ctx, p.x, p.y);
	}
	cairo_stroke(ctx);

	/* draw guidelines */
	cairo_set_source_rgb(ctx, 0.0, 0.0, 0.0);
	cairo_set_dash(ctx, &dash_pattern, 1, 0);
	cairo_move_to(ctx, p1.x, p1.y);
	cairo_line_to(ctx, p2.x, p2.y);
	cairo_stroke(ctx);

	cairo_move_to(ctx, p2.x, p2.y);
	cairo_line_to(ctx, p3.x, p3.y);
	cairo_stroke(ctx);

	cairo_set_font_size(ctx, 18);
	cairo_move_to(ctx, p1.x-10, p1.y-15);
	cairo_show_text(ctx, "P1");

	cairo_move_to(ctx, p2.x+10, p2.y+10);
	cairo_show_text(ctx, "P2");

	cairo_move_to(ctx, p3.x+10, p3.y-10);
	cairo_show_text(ctx, "P3");

	cairo_set_font_size(ctx, 30);
	cairo_move_to(ctx, 200, 40);
	cairo_show_text(ctx, "Quadratic Bezier Curve");

	cairo_surface_write_to_png(surface, "bezier.png");

	cairo_surface_destroy(surface);
	cairo_destroy(ctx);
	surface = NULL;
	ctx = NULL;
	return 0;
}

/* Quadratic Bezier Curve Implementation */
Point bezier(Point* p1, Point* p2, Point* p3, float t)
{
	Point p;
	float c1, c2, c3;
	c1 = 1.0 - 2.0 * t + t * t;
	c2 = 2.0 * t * ( 1.0 - t);
	c3 = t * t;

	p.x = c1*p1->x + c2*p2->x + c3*p3->x;
	p.y = c1*p1->y + c2*p2->y + c3*p3->y;
	return p;
}
