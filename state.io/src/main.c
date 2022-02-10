#include "main.h"






int main(){
	// srand(0);
	srand(time(0));
	struct ColorMixer *colormixer = ReadColorConfig("assets/color-config.txt");
	if (SDL_Init(SDL_INIT_VIDEO)<0) error(SDL_GetError());
	if (TTF_Init()<0) error(SDL_GetError());
	
	TTF_Font *font36=TTF_OpenFont("assets/IRNazaninBold.ttf", 36);
	if (!font36) error("can't open font IRNazaninBold.ttf");
	
	SDL_Window* window = SDL_CreateWindow("state.io", 20, 20, Width, Height, SDL_WINDOW_OPENGL);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	ReadPotions("assets/potion-config.txt", renderer);

	struct GameMap map;
	map.pos=0;
	map.states=0;


	map.n=12;
	map.nn=16;
	map.m=3;
	GenerateRandomMap(&map);
	// SaveMap(&map, "assets/maps/map7");
	LoadMap(&map, "assets/maps/map1");

	// potions[0].x=200;
	// potions[0].y=250;
	// SDL_Rect shit={200-PotionResolution/2, 250-PotionResolution/2, PotionResolution, PotionResolution};
	// potions[0].rect=shit;
	// // potions[0].owner=1;
	// potions[0].typ=1;

	// LoadGame(&map, username, "assets/saved-game");

	// MainGameProcess(window, renderer, &map, colormixer, username);
	
	char username[32];
	memset(username, 0, sizeof(username));

	int page=MenuMainMenuCode;
	while (1){
		if (page==MenuExitCode)
			break ;
		if (page==MenuMainMenuCode){
			page=MainMenu(window, renderer, font36);
			continue ;
		}
		if (page==MenuContinueGameCode){
			LoadGame(&map, username, "assets/saved-game");
			page=MainGameProcess(window, renderer, &map, colormixer, username);
			continue ;
		}
		if (page==MenuNewGameCode){
			page=NewGameMenu(window, renderer, username, font36);
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
			page=ChooseMapMenu(window, renderer, &map, font36);
			continue ;
		}
		if (page==MenuRandomMapCode){
			map.m=rand2(2, 7);
			map.n=rand2(m, 17);
			map.nn=rand2(n, n+6);
			GenerateRandomMap(&map);
			page=MenuPreviewMapCode;
			continue ;
		}
		if (page==MenuCustomMapCode){
			page=CustomGameMenu(window, renderer, &map, font36);
			continue ;
		}
		if (page==MenuStartGameCode){
			page=MainGameProcess(window, renderer, &map, colormixer, username);
			continue ;
		}
		if (page==MenuPreviewMapCode){
			// todo
			page=MenuExitCode;
			continue ;
		}
		error("unexpected exit-code from a menu ?!");
	}
	
	if (map.pos) free(map.pos);
	if (map.states) free(map.states);
	for (int i=1; i<=8; i++)
		SDL_DestroyTexture(potion_textures[i]);
	free(colormixer->C);
	free(colormixer->minC);
	free(colormixer->maxC);
	free(colormixer);
	TTF_CloseFont(font36);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
