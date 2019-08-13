#ifndef __SOLVER__
#define __SOLVER__
#define _N 4
typedef void (*Deriv)(float , float*, float* );
void ns_rk4(float x0, float state[], float h, Deriv deriv);
void ns_print_array(int N, float values[]);
#endif
