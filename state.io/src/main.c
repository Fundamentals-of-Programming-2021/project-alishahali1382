#include "main.h"


const int MaxParallelTroops=5;
const int TroopDelayTime=900; // how long until the next wave of troops get deployed
const double TroopPerSecond=0.86; // number of soldiers generated in normal state per second

int n, m, nn; // n: number of states    m: number of players    nn: n+"number of shit states"



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
	// srand(time(0));
	srand(0);
	struct ColorMixer *colormixer = ReadColorConfig("assets/color-config.txt");
	if (SDL_Init(SDL_INIT_VIDEO)<0) error(SDL_GetError());
	if (TTF_Init()<0) error(SDL_GetError());
	TTF_Font *font28=TTF_OpenFont("assets/IRNazaninBold.ttf", 28);
	SDL_Window* window = SDL_CreateWindow("state.io", 20, 20, Width, Height, SDL_WINDOW_OPENGL);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	n=10;
	nn=16;
	m=3;
	struct State *states=(struct State *)malloc(nn*sizeof(struct State));
	GenerateRandomMap(states);
	PrepareMap(states);
	// printf("prepared map\n");

	for (int i=0; i<m; i++) states[i].owner=i+1, states[i].cnt=InitialSoldierCount;


	// note: for debug
	// DeployTroop(states+0, states+1, 5);
	AddAttackQuery(states+0, states+3);
	AddAttackQuery(states+1, states+3);
	AddAttackQuery(states+2, states+1);


	int begining_of_time = SDL_GetTicks();
	int last_tick=SDL_GetTicks();
	while (1){
		int start_ticks = SDL_GetTicks();
		if (handleEvents()==EXIT) break;

		
		int dt=SDL_GetTicks()-last_tick;
		last_tick+=dt;
		
		ProcessAttackQueries(dt);
		ProcessTroops(dt);
		ProcessStates(states, dt);

		// printf("states[0].cnt=%d\n", states[0].cnt);

		
		DrawBackGround(renderer, states, colormixer);
		DrawTroops(renderer, troops, colormixer);
		DrawStates(renderer, states, colormixer, font28);
		
		
		char* buffer = malloc(sizeof(char) * 100);
		sprintf(buffer, "elapsed time: %dms   FPS: %d", start_ticks-begining_of_time, min(FPS, 1000/max(SDL_GetTicks()-start_ticks, 1)));
		stringRGBA(renderer, 5, 5, buffer, 0, 0, 255, 255);
		free(buffer);
		
		SDL_RenderPresent(renderer);
		
		SDL_Delay(max(1000/FPS-(SDL_GetTicks()-start_ticks), 0));
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
