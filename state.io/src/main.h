#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

typedef long long ll;

#define Width 800
#define Height 600
extern const int FPS;
extern const int StateRadius;
extern const int MinStateDistance; // maybe change it?
extern const int BorderLineWidth; // the thickness of the lines seperating states
extern const int MinStates, MaxStates;
extern const int MinPlayers, MaxPlayers;
extern const int InitialSoldierCount; // number of soldiers of each state when the game begins
extern const int MaxSoldierCount; // number of soldiers of eash state when the game begins
extern const double SoldierSpeed; // pixel per second

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

extern int n, nn, m;

struct State{
	int x, y; // center position
	int owner, cnt; // cnt: number of soldiers
};

int distance_state(struct State *A, struct State *B);

void GenerateRandomMap(struct State *states);

struct ColorMixer{
	int blank, empty_state, border_line;
	int *minC, *maxC, *C;
};

int getpartialcolor(struct ColorMixer *colormixer, struct State *state);

struct ColorMixer* ReadColorConfig(char *filename);



