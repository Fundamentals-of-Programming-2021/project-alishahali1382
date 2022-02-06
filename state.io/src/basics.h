#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL_image.h>
#include <SDL_ttf.h>


int swap(int *x, int *y);
int min(int x, int y);
int max(int x, int y);
double dmin(double x, double y);
double dmax(double x, double y);
void assert(int f);
int rand2(int l, int r);
int sq(int x);
int distance2(int x, int y, int xx, int yy);
void error(const char *error_message);
int rgb_to_int(int r, int g, int b);
