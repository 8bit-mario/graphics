#include<stdio.h>
#include "solver.h"

/* RK4 solver */
void ns_rk4(float x0, float y[], float h, Deriv f)
{
	int i;
	float dydx[_N], k1[_N], k2[_N], k3[_N], k4[_N], y1[_N];
	f(x0, y, dydx);
	for(i=0;i<_N;i++)
	{
		k1[i] = h*dydx[i];	
		y1[i] = y[i] + k1[i] * 0.5f;
	}
	f(x0+h*0.5f, y1, dydx);
	for(i=0;i<_N;i++)
	{
		k2[i] = h*dydx[i];
		y1[i] = y[i] + k2[i] * 0.5f;
	}
	f(x0+h*0.5f, y1, dydx);
	for(i=0;i<_N;i++)
	{
		k3[i] = h*dydx[i];
		y1[i] = y[i] + k3[i];
	}
	f(x0+h, y1, dydx);
	for(i=0;i<_N;i++)
	{
		k4[i] = h*dydx[i];
		y[i] = y[i] + (k1[i]+2.0f*k2[i]+2.0f*k3[i]+k4[i]) / 6.0f;
	}
}

/* utility function to print array */
void ns_print_array(int N, float values[])
{
	int i;
	for(i=0;i<N;i++)
	{
		printf("%.12f\t", values[i]);
	}
	printf("\n");
}
