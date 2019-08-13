#ifndef __GRAPHICS__
#define __GRAPHICS__

void ns_clear();
void ns_set_color(unsigned char r, unsigned char g, unsigned char b);
/* line DDA algorithm */
void ns_init(unsigned char* buffer, int w, int h);
void ns_plot(int x, int y);
void ns_line(int x1, int y1, int x2, int y2);
void ns_rect(int x, int y, int w, int h);

void write_ppm(unsigned char* buffer, const char* filename, int w, int h);
#endif
