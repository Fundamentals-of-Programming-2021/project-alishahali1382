#include "main.h"


const int MaxParallelTroops=5;
const int TroopDelayTime=900; // how long until the next wave of troops get deployed
const double TroopPerSecond=0.86; // number of soldiers generated in normal state per second

int n, m; // n: number of states    m: number of players



int selected_state=-1; // -1: no state is selected

const int EXIT = -1;
int GameHandleEvents(SDL_Window *window, struct State *states){
	SDL_Event event;
	while (SDL_PollEvent(&event)){
		if (event.type == SDL_QUIT)
			return EXIT;
		if (event.type == SDL_MOUSEBUTTONDOWN){
			if (event.button.button == SDL_BUTTON_LEFT){
				int x, y, id=-1;
				SDL_GetMouseState(&x, &y);
				for (int i=0; i<n && id==-1; i++)
					if (distance2(x, y, states[i].x, states[i].y)<=StateRadius*StateRadius) id=i;

				if (id==-1){
					// left click on random place de-selects?
					selected_state=-1;
					continue ;
				}
				// printf("clicked on state %d\n", id);
				if (selected_state==-1){
					// todo: in final release, this should be:
					// if (states[id].owner!=1) continue ; // ignore selecting mutual sides
					if (!states[id].owner) continue ; // ignore selecting mutual sides
					selected_state=id;
				}
				else if (selected_state!=id){
					AddAttackQuery(states+selected_state, states+id);
					selected_state=-1;
				}
				// if selected_state==id: do nothing
			}
			else if (event.button.button == SDL_BUTTON_RIGHT){
				selected_state=-1;
			}
		}
	}
	return 0;
}


int main(){
	// srand(0);
	srand(time(0));
	struct ColorMixer *colormixer = ReadColorConfig("assets/color-config.txt");
	if (SDL_Init(SDL_INIT_VIDEO)<0) error(SDL_GetError());
	if (TTF_Init()<0) error(SDL_GetError());
	TTF_Font *font28=TTF_OpenFont("assets/IRNazaninBold.ttf", 28);
	SDL_Window* window = SDL_CreateWindow("state.io", 20, 20, Width, Height, SDL_WINDOW_OPENGL);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	struct GameMap *map=(struct GameMap *)malloc(sizeof(struct GameMap));


	map->n=12;
	map->nn=16;
	map->m=3;
	GenerateRandomMap(map);
	
	PrepareMap(map);
	
	struct State *states=map->states;
	for (int i=0; i<m; i++) states[i].owner=i+1, states[i].cnt=InitialSoldierCount;

	int begining_of_time = SDL_GetTicks();
	int last_tick=SDL_GetTicks();
	while (1){
		int start_ticks = SDL_GetTicks();
		if (GameHandleEvents(window, states)==EXIT) break;

		
		int dt=SDL_GetTicks()-last_tick;
		last_tick+=dt;
		
		ProcessAttackQueries(dt);
		ProcessTroops(dt);
		ProcessStates(states, dt);

		// printf("states[0].cnt=%d\n", states[0].cnt);

		
		DrawBackGround(renderer, states, colormixer);
		DrawTroops(renderer, troops, colormixer);
		DrawStates(renderer, states, colormixer, font28);
		
		
		char* buffer = malloc(sizeof(char) * 80);
		sprintf(buffer, "elapsed time: %dms   FPS: %d", start_ticks-begining_of_time, min(FPS, 1000/max(SDL_GetTicks()-start_ticks, 1)));
		stringRGBA(renderer, 5, 5, buffer, 0, 0, 255, 255);
		free(buffer);
		
		SDL_RenderPresent(renderer);
		
		SDL_Delay(max(1000/FPS-(SDL_GetTicks()-start_ticks), 0));
	}
	FreeMap(map);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
