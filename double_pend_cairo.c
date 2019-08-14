#include<stdio.h>
#include<math.h>
#include<malloc.h>
#include<string.h>
#include<cairo/cairo.h>
#include "solver.h"

void f(float x, float y[], float dydx[]);
void draw_pendulum(cairo_t* ctx, float state[], int tx, int ty);
void draw_text(cairo_t* ctx, char text[], float state[], int tx, int ty, int p);

#define FRAME 5000
#define WIDTH 400
#define HEIGHT 250

#define M1 1.0f
#define M2 1.0f
#define L1 1.0f
#define L2 1.0f
#define G 9.8f

int main(int argc, char* argv[])
{
	/* initialize graphic system */
	cairo_surface_t* surface;
	cairo_t* ctx;

        surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, WIDTH, HEIGHT);
	ctx = cairo_create(surface);
	cairo_select_font_face(ctx, "Ubuntu Mono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD); 
	cairo_set_font_size(ctx, 12);

	/* initial state = {theta1, omega1, theta2, omega2} */
	float state1[4] = {1.560f, 0.0f, 0.8f, 0.0f}; /* pendulum 1 */
	float state2[4] = {1.565f, 0.0f, 0.8f, 0.0f}; /* pendulum 2 */

	char filename[100];
	char text[100];
	float x0 = 0.0f, h = 0.0167;
	for(int i = 0; i < FRAME; i++)
	{
		ns_rk4(x0, state1, h, f); /* solve first pendulum */
		ns_rk4(x0, state2, h, f); /* solve second pendulum */
		x0 += h;
		
		/* drawing code */
		cairo_set_source_rgb(ctx, 1.0, 1.0, 1.0);
		cairo_rectangle(ctx, 0, 0, WIDTH, HEIGHT);	
		cairo_fill(ctx);
		cairo_set_source_rgb(ctx, 0.5, 0.5, 0.5);
		cairo_rectangle(ctx, 200-30, 0, 60, 20);
		cairo_fill(ctx);
		cairo_set_source_rgb(ctx, 1.0, 0.0, 0.0);
		draw_pendulum(ctx, state1, 200, 20);
		cairo_set_source_rgb(ctx, 0.0, 1.0, 0.0);
		draw_pendulum(ctx, state2, 200, 20);

		cairo_set_source_rgb(ctx, 0.0, 0.0, 0.0);
		draw_text(ctx,text,state1,10,20,1);
		draw_text(ctx,text,state2,10,100,2);
		/* save each frame of animation to file */
		sprintf(filename, "test%04d.png", i);
		cairo_surface_write_to_png(surface, filename);
	}


	cairo_surface_destroy(surface);
	cairo_destroy(ctx);
	return 0;
}

/* differential equation describing pendulum motion */
void f(float x, float y[], float dydx[])
{
	float delta = y[2]-y[0];
	dydx[0] = y[1];
	dydx[1] = (M2*L1*y[1]*y[1]*sinf(delta)*cosf(delta) 
			+ M2*G*sinf(y[2])*cosf(delta) + M2*L2*y[3]*y[3]*sin(delta)
			- (M1+M2)*G*sinf(y[0])) 
			/ ((M1+M2)*L1 - M2*L1*cosf(delta)*cosf(delta));
	dydx[2] = y[3];
	dydx[3] = (-M2*L2*y[3]*y[3]*sinf(delta)*cosf(delta)
		+(M1+M2)*(G*sinf(y[0])*cosf(delta)-L1*y[1]*y[1]*sinf(delta)-G*sinf(y[2])))
		/((M1+M2)*L2 - M2*L2*cosf(delta)*cosf(delta)); 
}

/* code to draw pendulum shape */
void draw_pendulum(cairo_t* ctx, float state[], int tx, int ty)
{
	float xp, yp, xp1, yp1;
	xp = tx + 100.0f * sinf(state[0]);
	yp = ty + 100.0f * cosf(state[0]);
	xp1 = xp + 100.0f * sinf(state[2]);
	yp1 = yp + 100.0f * cosf(state[2]);

	cairo_move_to(ctx, tx, ty);
	cairo_line_to(ctx, xp, yp);
	cairo_line_to(ctx, xp1, yp1);
	cairo_stroke(ctx);
	cairo_arc(ctx, xp, yp, 5.0, 0.0, 2*M_PI);
	cairo_arc(ctx, xp1, yp1, 5.0, 0.0, 2*M_PI);
	cairo_fill(ctx);
	//ns_line(tx, ty, xp, yp);
	//ns_line(xp, yp, xp1, yp1);
	//ns_rect(xp - 4, yp - 4, 8, 8);
	//ns_rect(xp1 - 4, yp1 - 4, 8, 8);
}

/* code to draw text */
void draw_text(cairo_t* ctx, char text[], float state[], int tx, int ty, int p)
{
	float E;
	E = 0.5*M1*L1*L1*state[1]*state[1]+
		0.5*M2*(L1*L1*state[1]*state[1] +
			L2*L2*state[3]*state[3]+
			2.0*L1*L2*state[1]*state[3]*cosf(state[0]-state[2]))
	     -(M1+M2)*G*L1*cosf(state[0])-M2*G*L2*cosf(state[2]);	
	cairo_move_to(ctx, tx, ty);
	if(p == 1)
		cairo_show_text(ctx, "Red pendulum");
	else
		cairo_show_text(ctx, "Green pendulum");
	cairo_move_to(ctx, tx, ty+14);
	sprintf(text, "\tθ₁ = %.2f", state[0]*180.0/M_PI);
	cairo_show_text(ctx, text);	
	sprintf(text, "\tω₁ = %.2f", state[1]);
	cairo_move_to(ctx, tx, ty+26);
	cairo_show_text(ctx, text);
	cairo_move_to(ctx, tx, ty+38);
	sprintf(text, "\tθ₂ = %.2f", state[2]*180.0/M_PI);
	cairo_show_text(ctx, text);	
	sprintf(text, "\tω₂ = %.2f", state[3]);
	cairo_move_to(ctx, tx, ty+50);
	cairo_show_text(ctx, text);
	cairo_move_to(ctx, tx, ty+62);
	sprintf(text, "\tE = %.6f", E);
	cairo_show_text(ctx, text);
}
