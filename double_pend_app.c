#include<SDL2/SDL.h>
#include<cairo/cairo.h>
#include<stdlib.h>
#include<math.h>
#include<unistd.h>
#include<time.h>
#include "solver.h"

#define NS_BUTTON_DOWN 1
#define NS_BUTTON_OVER 2
#define NS_BUTTON_NORMAL 3

typedef struct _Button
{
	char text[100];
	int id;
	int x, y;
	int width, height;
	int state;
} Button;

typedef void (*Callback)(Button*);
#define FPS 60
#define WIDTH 800
#define HEIGHT 600

#define M1 1.0f /* mass of pendulum */
#define M2 1.0f
#define L1 1.0f /* length of pendulum */
#define L2 1.0f
#define G 9.8f /* accel due to gravity */
#define P1_X 250 
#define P1_Y 50
#define P2_X 550
#define P2_Y 50

void f(float, float[], float[]);
void draw_pendulum(cairo_t*, float[], int, int);
void draw_text(cairo_t*, char[], float[], int, int, int);
void draw_button(cairo_t* ctx, Button* btn);
void process_btn_event(const SDL_Event*,  Button*, Callback);
void process_pendulum_event(SDL_Event*);
void btn1_callback(Button* );
void btn2_callback(Button* );

float cool_x, cool_y;
int play = 0;

/* initialize state of pendulum */
/* initial state = {theta1, omega1, theta2, omega2} */
float state1[4] = {1.560f, 0.0f, 0.8f, 0.0f}; /* pendulum 1 */
float state2[4] = {1.56001f, 0.0f, 0.8f, 0.0f}; /* pendulum 2 */
int main(int argc, char *argv[])
{
	SDL_Window *win;	
	SDL_Surface* surf;
	cairo_t* ctx;
	cairo_surface_t* csurf;

	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Error initializing SDL_Init(): %s", SDL_GetError());
		return 1;
	}

	win = SDL_CreateWindow("Double Pendulum", 0, 0, 800, 600, 0);	
	surf = SDL_GetWindowSurface(win);


	/* initialize cairo rendering engine */
	csurf = cairo_image_surface_create_for_data(surf->pixels,
			CAIRO_FORMAT_RGB24,
			surf->w,
			surf->h,
			surf->pitch);
	ctx = cairo_create(csurf);
	cairo_select_font_face(ctx, "Ubuntu Mono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(ctx, 20);
	cairo_set_line_width(ctx, 1.5);
	
	char text[100];
	float x0 = 0.0f, h = 1.0f / (float)FPS;

	Button btn1 = {"Play", 1, 10, 320, 80, 40, 3};
	Button btn2 = {"Reset", 2, 10, 370, 80, 40, 3};
	cool_x = WIDTH*0.5;
	cool_y = HEIGHT-150;

	SDL_Event evt;
	int start, diff;
	int done = 0, frame = 0;
	int mx, my;
	mx = 0; my = 0;
	/* main loop */
	while(!done)
	{
		start = SDL_GetTicks();
		/* Process pending events */
		while(SDL_PollEvent(&evt))
		{
			process_btn_event(&evt, &btn1, btn1_callback);
			process_btn_event(&evt, &btn2, btn2_callback);
			if(!play)
				process_pendulum_event(&evt);
			switch(evt.type)
			{
				case SDL_QUIT:
					done = 1;
					break;
				case SDL_KEYDOWN:
					if(evt.key.keysym.sym == SDLK_ESCAPE)
					{
						done = 1;
					}
					break;
				case SDL_MOUSEMOTION:
					mx = evt.motion.x;
					my = evt.motion.y;
					
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
			}
		}

		/* update */
		if(play)
		{
			ns_rk4(x0, state1, h, f);
			ns_rk4(x0, state2, h, f);
			x0 += h;
		}
		cool_x += (mx - cool_x)*0.08;
		if(cool_x < 100) cool_x = 100;
		if(cool_x > WIDTH-100) cool_x = WIDTH - 100;


		/* clear screen with white color */
		cairo_set_source_rgb(ctx, 1.0, 1.0, 1.0);
		cairo_paint(ctx);

		/* draw */
		cairo_set_source_rgb(ctx, 1.0, 0.0, 0.0);
		draw_pendulum(ctx, state1, P1_X, P1_Y);
		cairo_set_source_rgb(ctx, 0.0, 1.0, 0.0);
		draw_pendulum(ctx, state2, P2_X, P2_Y);

		cairo_set_source_rgb(ctx, 0.0, 0.0, 0.0);
		draw_text(ctx,text,state1,180,320,1);
		draw_text(ctx,text,state2,500,320,2);

		draw_button(ctx, &btn1);
		draw_button(ctx, &btn2);

		cairo_rectangle(ctx, 10, cool_y, WIDTH-20, 140);
		cairo_stroke(ctx);
		cairo_rectangle(ctx, cool_x-50, cool_y-10, 100, 20);
		cairo_fill(ctx);
		cairo_move_to(ctx, 50, cool_y+40);
		cairo_show_text(ctx, "Usage: Press Pause button to play the simulation. ");
		cairo_move_to(ctx, 50, cool_y+60);
		cairo_show_text(ctx, "Press Play button to pause, use mouse to drag pendulum to new position.");
		cairo_move_to(ctx, 50, cool_y+80);
		cairo_show_text(ctx, "Press Reset to reset the state of pendulum");
		cairo_move_to(ctx, 50, cool_y+120);
		cairo_show_text(ctx, "App develop by Niraj Shrestha");


		/* show the surface on screen */
		SDL_UpdateWindowSurface(win);

		/* cap the frame rate to 60 fps */
		diff = SDL_GetTicks() - start;
		if(diff < 1000.0/FPS)
			SDL_Delay(1000.0/FPS - diff);

		frame++;
	}

	/* clean up code */
	cairo_surface_destroy(csurf);
	cairo_destroy(ctx);
	SDL_DestroyWindow(win);
	SDL_Quit();

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

void btn1_callback(Button* btn)
{
	if(play == 1)
		strcpy(btn->text, "Play");
	else
		strcpy(btn->text, "Pause");
	play = !play;
}

void btn2_callback(Button* btn)
{
	/*
	state1[0] = 1.560f;
	state1[1] = 0.0f;
	state1[2] = 0.8f;
	state1[3] = 0.0f;

	state2[0] = 1.56001f;
	state2[1] = 0.0f;
	state2[2] = 0.8f;
	state2[3] = 0.0f;
	*/	
	state1[0] = 0.0f;
	state1[1] = 0.0f;
	state1[2] = 0.0f;
	state1[3] = 0.0f;

	state2[0] = 0.0f;
	state2[1] = 0.0f;
	state2[2] = 0.0f;
	state2[3] = 0.0f;
}

/* code to draw pendulum shape */
void draw_pendulum(cairo_t* ctx, float state[], int tx, int ty)
{
	float xp, yp, xp1, yp1;
	xp = tx + 120.0f * sinf(state[0]);
	yp = ty + 120.0f * cosf(state[0]);
	xp1 = xp + 120.0f * sinf(state[2]);
	yp1 = yp + 120.0f * cosf(state[2]);

	cairo_rectangle(ctx, tx - 30, ty-5, 60, 10);
	cairo_fill(ctx);
	cairo_move_to(ctx, tx, ty);
	cairo_line_to(ctx, xp, yp);
	cairo_line_to(ctx, xp1, yp1);
	cairo_stroke(ctx);
	cairo_arc(ctx, xp, yp, 8.0, 0.0, 2*M_PI);
	cairo_arc(ctx, xp1, yp1, 8.0, 0.0, 2*M_PI);
	cairo_fill(ctx);
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
	cairo_move_to(ctx, tx, ty+20);
	sprintf(text, "\tθ₁ = %.2f", state[0]*180.0/M_PI);
	cairo_show_text(ctx, text);	
	sprintf(text, "\tω₁ = %.2f", state[1]);
	cairo_move_to(ctx, tx, ty+40);
	cairo_show_text(ctx, text);
	cairo_move_to(ctx, tx, ty+60);
	sprintf(text, "\tθ₂ = %.2f", state[2]*180.0/M_PI);
	cairo_show_text(ctx, text);	
	sprintf(text, "\tω₂ = %.2f", state[3]);
	cairo_move_to(ctx, tx, ty+80);
	cairo_show_text(ctx, text);
	cairo_move_to(ctx, tx, ty+100);
	sprintf(text, "\tE = %.6f", E);
	cairo_show_text(ctx, text);
}

void draw_button(cairo_t* ctx, Button* btn)
{
	cairo_text_extents_t ext;
	if(btn->state == NS_BUTTON_NORMAL)
		cairo_set_source_rgb(ctx, 0.2, 0.8, 0.4);
	else if(btn->state == NS_BUTTON_OVER)
		cairo_set_source_rgb(ctx, 1.0, 1.0, 0.0);
	else
		cairo_set_source_rgb(ctx, 1.0, 1.0, 0.5);
	cairo_rectangle(ctx, btn->x, btn->y, btn->width, btn->height);
	cairo_fill(ctx);
	cairo_set_source_rgb(ctx, 0.0, 0.0, 0.0);
	cairo_text_extents(ctx, btn->text, &ext);
	cairo_move_to(ctx, btn->x+btn->width*0.5-ext.width*0.5, btn->y+btn->height*0.6);
	cairo_show_text(ctx, btn->text);
}

void process_btn_event(const SDL_Event* evt, Button* btn, Callback c)
{
	int mx, my;
	mx = evt->motion.x;
	my = evt->motion.y;
	switch(evt->type)
	{
		case SDL_MOUSEBUTTONDOWN:
			if((mx > btn->x) && (mx < btn->x + btn->width) && (my > btn->y) && (my < btn->y+btn->height))
				btn->state = NS_BUTTON_DOWN;
			break;
		case SDL_MOUSEBUTTONUP:
			if((mx > btn->x) && (mx < btn->x + btn->width) && (my > btn->y) && (my < btn->y+btn->height))
			{
				btn->state = NS_BUTTON_OVER;
				if(c != NULL)
				c(btn);
			}
			break;
		case SDL_MOUSEMOTION:
			if((mx > btn->x) && (mx < btn->x + btn->width) && (my > btn->y) && (my < btn->y+btn->height))
			{
				btn->state = NS_BUTTON_OVER;
			}
			else
			{
				btn->state = NS_BUTTON_NORMAL;
			}
			break;
	}
}

int pend_click = 0;
int pend_ball = -1;
float x[4], y[4];
void process_pendulum_event(SDL_Event* evt)
{
	int mx, my;
	float dst_sq[4];
	float min_dst = 9000;
	mx = evt->motion.x;
	my = evt->motion.y;

	switch(evt->type)
	{
		case SDL_MOUSEBUTTONDOWN:
			pend_click = 1;	
			x[0] = P1_X+120.0f*sinf(state1[0]);
			y[0] = P1_Y+120.0f*cosf(state1[0]);
			dst_sq[0] = (mx-x[0])*(mx-x[0])+(my-y[0])*(my-y[0]);	
			x[1] = x[0]+120.0f*sinf(state1[2]);
			y[1] = y[0]+120.0f*cosf(state1[2]);
			dst_sq[1] = (mx-x[1])*(mx-x[1])+(my-y[1])*(my-y[1]);	
			x[2] = P2_X+120.0f*sinf(state2[0]);
			y[2] = P2_Y+120.0f*cosf(state2[0]);
			dst_sq[2] = (mx-x[2])*(mx-x[2])+(my-y[2])*(my-y[2]);	
			x[3] = x[2]+120.0f*sinf(state2[2]);
			y[3] = y[2]+120.0f*cosf(state2[2]);
			dst_sq[3] = (mx-x[3])*(mx-x[3])+(my-y[3])*(my-y[3]);	


			int index;
			for(int i = 0; i < 4; i++)
			{
				if(min_dst > dst_sq[i])
				{
					min_dst = dst_sq[i];
					index = i;
				}	
			}
			if(dst_sq[index] < 20*20)
			{
				pend_ball = index;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			pend_click = 0;
			pend_ball = -1;
			break;
		case SDL_MOUSEMOTION:
			if(pend_click && pend_ball >= 0)
			{
				int dx, dy;
				if(pend_ball == 0) 
				{
					dx = (mx - P1_X);
					dy = (my - P1_Y);
				       	state1[0] = atan2f(dx, dy);
				}
				else if(pend_ball == 1)
				{
					dx = (mx - x[0]);
					dy = (my - y[0]);
					state1[2] = atan2f(dx, dy);
				}
				else if(pend_ball == 2)
				{
					dx = (mx - P2_X);
					dy = (my - P2_Y);
				       	state2[0] = atan2f(dx, dy);
				}
				else
				{
					dx = (mx - x[2]);
					dy = (my - y[2]);
					state2[2] = atan2f(dx, dy);
				}
			}
			break;
	}
}
