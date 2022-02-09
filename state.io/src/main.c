#include "main.h"






int main(){
	// srand(0);
	srand(time(0));
	struct ColorMixer *colormixer = ReadColorConfig("assets/color-config.txt");
	if (SDL_Init(SDL_INIT_VIDEO)<0) error(SDL_GetError());
	if (TTF_Init()<0) error(SDL_GetError());
	SDL_Window* window = SDL_CreateWindow("state.io", 20, 20, Width, Height, SDL_WINDOW_OPENGL);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	ReadPotions("assets/potion-config.txt", renderer);

	struct GameMap map;


	map.n=12;
	map.nn=16;
	map.m=3;
	map.pos=0;
	map.states=0;
	GenerateRandomMap(&map);
	// SaveMap(&map, "assets/maps/map7");
	LoadMap(&map, "assets/maps/map1");

	potions[0].x=200;
	potions[0].y=250;
	SDL_Rect shit={200-PotionResolution/2, 250-PotionResolution/2, PotionResolution, PotionResolution};
	potions[0].rect=shit;
	// potions[0].owner=1;
	potions[0].typ=1;
	

	MainGameProcess(window, renderer, &map, colormixer);
	

	/*
	int page=MenuMainMenuCode;
	// page=MenuNewGameCode;
	while (1){
		if (page==MenuExitCode)
			break ;
		if (page==MenuMainMenuCode){
			page=MainMenu(window, renderer);
			continue ;
		}
		if (page==MenuContinueGameCode){
			// todo
			page=MenuExitCode;
			continue ;
		}
		if (page==MenuNewGameCode){
			page=NewGameMenu(window, renderer);
			continue ;
		}
		if (page==MenuLeaderboardCode){
			// todo
			page=MenuExitCode;
			continue ;
		}
		if (page==MenuCreditCode){
			// todo
			page=MenuExitCode;
			continue ;
		}
		if (page==MenuChooseMapCode){
			// todo
			page=MenuExitCode;
			continue ;
		}
		if (page==MenuRandomMapCode){
			// todo
			page=MenuExitCode;
			continue ;
		}
		if (page==MenuCustomMapCode){
			// todo
			page=MenuExitCode;
			continue ;
		}
		assert(0);
	}
	*/

	for (int i=1; i<=8; i++)
		SDL_DestroyTexture(potion_textures[i]);
	free(colormixer->C);
	free(colormixer->minC);
	free(colormixer->maxC);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
