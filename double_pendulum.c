#include<stdio.h>
#include<math.h>
#include<malloc.h>
#include<string.h>
#include "graphics.h"
#include "solver.h"

void f(float x, float y[], float dydx[]);
void draw_pendulum(float state[], int tx, int ty);

#define FRAME 1000
#define WIDTH 400
#define HEIGHT 250

#define M1 1.0f
#define M2 1.0f
#define L1 1.0f
#define L2 1.0f
#define G 9.8f

int main(int argc, char* argv[])
{
	unsigned char* buffer = (unsigned char*)malloc(WIDTH*HEIGHT*3);
	/* initialize graphic system */
	ns_init(buffer, WIDTH, HEIGHT);

	/* initial state = {theta1, omega1, theta2, omega2} */
	float state1[4] = {1.560f, 0.0f, 0.8f, 0.0f}; /* pendulum 1 */
	float state2[4] = {1.565f, 0.0f, 0.8f, 0.0f}; /* pendulum 2 */

	char filename[100];
	float x0 = 0.0f, h = 0.0167;
	for(int i = 0; i < FRAME; i++)
	{
		ns_rk4(x0, state1, h, f); /* solve first pendulum */
		ns_rk4(x0, state2, h, f); /* solve second pendulum */
		x0 += h;
		
		/* drawing code */
		ns_clear();
		ns_set_color(0, 0, 255);
		ns_rect(200-50, 10, 100, 10);
		ns_set_color(255, 0, 0);
		draw_pendulum(state1, 200, 20);
		ns_set_color(0, 255, 0);
		draw_pendulum(state2, 200, 20);

		/* save each frame of animation to file */
		sprintf(filename, "test%03d.ppm", i);
		write_ppm(buffer, filename, WIDTH, HEIGHT);	
	}
	free(buffer);
	return 0;
}

/* code to draw pendulum shape */
void draw_pendulum(float state[], int tx, int ty)
{
	float xp, yp, xp1, yp1;
	xp = tx + 100.0f * sinf(state[0]);
	yp = ty + 100.0f * cosf(state[0]);
	xp1 = xp + 100.0f * sinf(state[2]);
	yp1 = yp + 100.0f * cosf(state[2]);

	ns_line(tx, ty, xp, yp);
	ns_line(xp, yp, xp1, yp1);
	ns_rect(xp - 4, yp - 4, 8, 8);
	ns_rect(xp1 - 4, yp1 - 4, 8, 8);
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
