#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#define Width 800
#define Height 600
const double FPS = 30;
const int MinStates=6, MaxStates=30;
const int MinPlayers=2, MaxPlayers=6;
const int MinStateDistance=70; // maybe change it?

// here are some shit functions C doesnt have :)
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


int n, m, nn; // n: number of states    m: number of players    nn: n+"number of shit states"
struct State{
	int x, y; // center position

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
int A[Width][Height]; // global array needed for making background
SDL_Texture* MakeBackGround(SDL_Renderer *renderer, struct State *states){
	SDL_Texture *texture=SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, Width, Height);
	SDL_SetRenderTarget(renderer, texture);

	// SDL_SetRenderDrawColor(renderer, 120, 60, 80, 255);
	// SDL_RenderClear(renderer);
	
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
		A[x][y]=min(n, bst);
	}
	
	int *color=(int *) malloc((n+1)*sizeof(int));
	for (int i=0; i<n; i++) color[i]=0xff000000+rand()%01000000;
	color[n]=0xffdfb320;
	for (int x=0; x<Width; x++) for (int y=0; y<Height; y++){
		if (x<=0 || y<=0 || Width-1<=x || Height-1<=y)
			pixelColor(renderer, x, y, 0xff000000);
		else{
			int f=0;
			for (int i=-1; i<=1; i++) for (int j=-1; j<=1; j++){
				if (A[x+i][y+j]==A[x][y]) continue ;
				f=1;
				// maybe use different condition here?
			}
			if (f) pixelColor(renderer, x, y, 0xff000000);
			else pixelColor(renderer, x, y, color[A[x][y]]);
		}
	}
	free(color); // free-ed 60 bytes :))


	for (int i=0; i<nn; i++){
		filledCircleColor(renderer, states[i].x, states[i].y, 15, 0xff00ff00);
		// printf("circle %d is at position (%d, %d)\n", i, states[i].x, states[i].y);
	}

	SDL_SetRenderTarget(renderer, 0);
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
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("map generator", 20, 20, Width, Height, SDL_WINDOW_OPENGL);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	

	n=10;
	nn=15;
	m=2;
	struct State *states=(struct State *)malloc(nn*sizeof(struct State));
	GenerateRandomMap(states);



	SDL_Texture *background=MakeBackGround(renderer, states);

	int begining_of_time = SDL_GetTicks();
	while (1) {
		int start_ticks = SDL_GetTicks();

		if (handleEvents()==EXIT) break;

		SDL_RenderCopy(renderer, background, 0, 0); // draw screen background map
		
		char* buffer = malloc(sizeof(char) * 50);
		sprintf(buffer, "elapsed time: %dms", start_ticks-begining_of_time);
		// printf("%s", buffer);
		stringRGBA(renderer, 5, 5, buffer, 0, 0, 255, 255);
		
		SDL_RenderPresent(renderer);

		// SDL_Delay(1000/FPS-(SDL_GetTicks()-start_ticks));
		while (SDL_GetTicks()-start_ticks < 1000 / FPS);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
