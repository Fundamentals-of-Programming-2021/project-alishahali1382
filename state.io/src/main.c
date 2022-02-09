#include "main.h"


const int MaxParallelTroops=5;
const int TroopDelayTime=900; // how long until the next wave of troops get deployed
const double TroopPerSecond=0.86; // number of soldiers generated in normal state per second





int main(){
	// srand(0);
	srand(time(0));
	struct ColorMixer *colormixer = ReadColorConfig("assets/color-config.txt");
	if (SDL_Init(SDL_INIT_VIDEO)<0) error(SDL_GetError());
	if (TTF_Init()<0) error(SDL_GetError());
	SDL_Window* window = SDL_CreateWindow("state.io", 20, 20, Width, Height, SDL_WINDOW_OPENGL);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	struct GameMap map;


	map.n=12;
	map.nn=16;
	map.m=3;
	GenerateRandomMap(&map);
	
	MainMenu(window, renderer);
	// MainGameProcess(window, renderer, &map, colormixer);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
