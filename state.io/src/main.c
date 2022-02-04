#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
// #include "main.h"
// #include "state.c"

typedef long long ll;
#define Width 800
#define Height 600
const int FPS = 30;
const int MinStateDistance=70; // maybe change it?
const int BorderLineWidth=2; // the thickness of the lines seperating states
const int MinStates=6, MaxStates=30;
const int MinPlayers=2, MaxPlayers=6;
const int InitialSoldierCount=10; // number of soldiers of eash state when the game begins
const int MaxSoldierCount=50; // number of soldiers of eash state when the game begins

int swap(int *x, int *y){ *x^=*y, *y^=*x, *x^=*y;}
int min(int x, int y){ return (x<y?x:y);}
int max(int x, int y){ return (x>y?x:y);}
double dmin(double x, double y){ return (x<y?x:y);}
double dmax(double x, double y){ return (x>y?x:y);}
void assert(int f){ f=1/f;}
int rand2(int l, int r){ return rand()%(r-l)+l;} // random number in [l, r)
int sq(int x){ return x*x;}
int distance2(int x, int y, int xx, int yy){ // squared distance
	int dx=x-xx, dy=y-yy;
	return dx*dx+dy*dy;
}
void error(char *error_message){
	FILE *f=fopen("error-log.txt", "w+");
	fprintf(f, "%s\n", error_message);
	fclose(f);
	exit(1);
}
int rgb_to_int(int r, int g, int b){ return 0xff000000+(b<<16)+(g<<8)+(r);}


int n, m, nn; // n: number of states    m: number of players    nn: n+"number of shit states"
struct State{
	int x, y; // center position
	int owner, cnt; // cnt: number of soldiers
};

int distance_state(struct State *A, struct State *B){ // squared distance
	int dx=(A->x)-(B->x), dy=(A->y)-(B->y);
	return dx*dx+dy*dy;
}

void GenerateRandomMap(struct State *states){ // uses global variable nn
	for (int i=0; i<nn; i++){
		(states+i)->x=rand2(MinStateDistance/2, Width-MinStateDistance/2);
		(states+i)->y=rand2(MinStateDistance/2, Height-MinStateDistance/2);
		int bad=0;
		for (int j=0; j<i && !bad; j++)
			if (distance_state(states+i, states+j)<=MinStateDistance*MinStateDistance)
				bad=1;
		
		if (bad) i--;
	}
}

struct ColorMixer{
	int blank, empty_state, border_line;
	int *minC, *maxC, *C;
};
int getpartialcolor(struct ColorMixer *colormixer, struct State *state){
	if (!(state->owner)) return colormixer->empty_state;
	int ted=min(state->cnt, MaxSoldierCount);
	int mn=colormixer->minC[state->owner]-0xff000000;
	int mx=colormixer->maxC[state->owner]-0xff000000;
	int r=((mn&0xff)*(MaxSoldierCount-ted) + (mx&0xff)*ted)/MaxSoldierCount; mn>>=8; mx>>=8;
	int g=((mn&0xff)*(MaxSoldierCount-ted) + (mx&0xff)*ted)/MaxSoldierCount; mn>>=8; mx>>=8;
	int b=((mn&0xff)*(MaxSoldierCount-ted) + (mx&0xff)*ted)/MaxSoldierCount; mn>>=8; mx>>=8;
	// note: maybe just (mn*a+mx*b)/(a+b)?
	return r+(g<<8)+(b<<16)+0xff000000; // note: maybe handle alpha manually?
}

struct ColorMixer* ReadColorConfig(char *filename){
	FILE *f=fopen(filename, "r");
	if (f==NULL) error("color config file not found :(");
	
	struct ColorMixer *res=(struct ColorMixer *)(malloc(sizeof(struct ColorMixer)));
	res->C=(int*)(malloc(sizeof(int)*(MaxPlayers+1)));
	res->minC=(int*)(malloc(sizeof(int)*(MaxPlayers+1)));
	res->maxC=(int*)(malloc(sizeof(int)*(MaxPlayers+1)));
	
	// memory leak doesn't matter here since its called only once
	char S[50];
	int r, g, b;
	int tmp=fscanf(f, "%s %d,%d,%d", S, &r, &g, &b);
	if (tmp<4 || strcmp(S, "blank:")) error("color-config file invalid line 1 blank");
	res->blank=rgb_to_int(r, g, b);

	tmp=fscanf(f, "%s %d,%d,%d", S, &r, &g, &b);
	if (tmp<4 || strcmp(S, "border-line:")) error("color-config file invalid line 2 border-line");
	res->border_line=rgb_to_int(r, g, b);
	
	tmp=fscanf(f, "%s %d,%d,%d", S, &r, &g, &b);
	if (tmp<4 || strcmp(S, "empty-state:")) error("color-config file invalid line 3 empty-state");
	res->empty_state=rgb_to_int(r, g, b);

	char Smin[]="player0-min:", Smax[]="player0-max:", SS[]="player0:";
	char error_message[]="color-config file invalid player 0 color";
	for (int player=1; player<=MaxPlayers; player++){
		// MaxPlayers<=9
		SS[6]++;
		Smin[6]++;
		Smax[6]++;
		error_message[33]++;
		
		tmp=fscanf(f, "%s %d,%d,%d", S, &r, &g, &b);
		if (tmp<4 || strcmp(S, SS)) error(error_message);
		res->C[player]=rgb_to_int(r, g, b);
		
		tmp=fscanf(f, "%s %d,%d,%d", S, &r, &g, &b);
		if (tmp<4 || strcmp(S, Smin)) error(error_message);
		res->minC[player]=rgb_to_int(r, g, b);
		
		tmp=fscanf(f, "%s %d,%d,%d", S, &r, &g, &b);
		if (tmp<4 || strcmp(S, Smax)) error(error_message);
		res->maxC[player]=rgb_to_int(r, g, b);
	}
	fclose(f);
	return res;
}

// A[x][y]=-1   : its a border line
// 0<=A[x][y]<n : its a state
// A[x][y]=n    : its a non-playing field
int A[Width][Height], B[Width][Height]; // global array needed for making background
void PrepareMap(struct State *states){
	if (!n) error("PrepareMap called with n=0");
	for (int x=0; x<Width; x++) for (int y=0; y<Height; y++){
		int bst=-1, val=1e9;
		for (int i=0; i<nn; i++){
			int d=distance2(x, y, states[i].x, states[i].y);
			if (d<val){
				val=d;
				bst=i;
			}
		}
		assert(bst!=-1);
		B[x][y]=min(n, bst);
	}
	for (int x=0; x<Width; x++) for (int y=0; y<Height; y++){
		if (x<BorderLineWidth || y<BorderLineWidth || Width-BorderLineWidth<=x || Height-BorderLineWidth<=y)
			A[x][y]=-1;
		else{
			A[x][y]=B[x][y];
			for (int i=-BorderLineWidth; i<=BorderLineWidth; i++) for (int j=-BorderLineWidth; j<=BorderLineWidth; j++){
				if (B[x+i][y+j]==B[x][y]) continue ;
				A[x][y]=-1;
				// maybe use different condition here?
			}
		}
	}
	ll sumx[n], sumy[n], ted[n];
	memset(sumx, 0, sizeof(sumx));
	memset(sumy, 0, sizeof(sumy));
	memset(ted, 0, sizeof(ted));
	for (int x=0; x<Width; x++) for (int y=0; y<Height; y++) if (0<=A[x][y] && A[x][y]<n){
		sumx[A[x][y]]+=x;
		sumy[A[x][y]]+=y;
		ted[A[x][y]]++;
	}
	for (int i=0; i<n; i++){
		// assert(ted[i]);
		states[i].x=sumx[i]/ted[i];
		states[i].y=sumy[i]/ted[i];
		states[i].owner=0;
		states[i].cnt=InitialSoldierCount;
	}
}

SDL_Texture* MakeBackGround(SDL_Renderer *renderer, struct State *states, struct ColorMixer *colormixer){
	// it makes a texture that corresponds to background.
	// the main point is optimizing the renderings and dont create a background at each frame
	// a new backgroung should be created every time a cells owner changes.
	// if partial-color is used, the process is mostly point-less
	SDL_Surface *surface=SDL_CreateRGBSurface(0, Width, Height, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

	int color[n];
	for (int i=0; i<n; i++) color[i]=getpartialcolor(colormixer, states+i);

	int *pixel=surface->pixels;
	for (int y=0; y<Height; y++)for (int x=0; x<Width; x++) {
		if (A[x][y]==-1) *pixel=colormixer->border_line;
		else if (A[x][y]==n) *pixel=colormixer->blank;
		else *pixel=color[A[x][y]];
		pixel++;
		// if (A[x][y]==-1) pixelColor(renderer, x, y, colormixer->border_line);
		// else if (A[x][y]==n) pixelColor(renderer, x, y, colormixer->blank);
		// else pixelColor(renderer, x, y, color[A[x][y]]);
		// else pixelColor(renderer, x, y, getpartialcolor(colormixer, states+A[x][y]));
		// note: edit here later
	}
	SDL_Texture *texture=SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}


const int EXIT = -1;
int handleEvents(){
	SDL_Event event;
	while (SDL_PollEvent(&event)){
		if (event.type == SDL_QUIT)
			return EXIT;
	}
	return 0;
}


int main(){
	srand(time(0));
	struct ColorMixer *colormixer = ReadColorConfig("assets/color-config.txt");
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("state.io", 20, 20, Width, Height, SDL_WINDOW_OPENGL);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	n=10;
	nn=20;
	m=6;
	struct State *states=(struct State *)malloc(nn*sizeof(struct State));
	GenerateRandomMap(states);
	PrepareMap(states);
	// printf("prepared map\n");

	for (int i=0; i<m; i++) states[i].owner=i+1, states[i].cnt=InitialSoldierCount;

	SDL_Texture *background;

	int begining_of_time = SDL_GetTicks();
	printf("begining_of_time=%d\n", begining_of_time);
	while (1){
		int start_ticks = SDL_GetTicks();
		if (handleEvents()==EXIT) break;

		if (background) SDL_DestroyTexture(background);
		background=MakeBackGround(renderer, states, colormixer);
		printf("render background done in %dms\n", SDL_GetTicks()-start_ticks);

		SDL_RenderCopy(renderer, background, 0, 0); // draw screen background map
		
		
		char* buffer = malloc(sizeof(char) * 60);
		sprintf(buffer, "elapsed time: %dms   FPS: %d", start_ticks-begining_of_time, 1000/max(SDL_GetTicks()-start_ticks, 1));
		stringRGBA(renderer, 5, 5, buffer, 0, 0, 255, 255);
		
		
		SDL_RenderPresent(renderer);




		// printf("delay=%d\n", 1000/FPS-(SDL_GetTicks()-start_ticks));
		// int tmp=SDL_GetTicks();
		// printf("GetTicks=%d\n", tmp);
		
		SDL_Delay(max(1000/FPS-(SDL_GetTicks()-start_ticks), 0));
		// while (SDL_GetTicks()-start_ticks < 1000 / FPS);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
