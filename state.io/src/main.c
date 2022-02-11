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
	map.pos=NULL;
	map.states=NULL;

	char username[32]="admin";
	
	int page=MenuMainMenuCode;
	// page=MenuNewGameCode;
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
			ResetGame();
			page=ChooseMapMenu(window, renderer, &map, font36);
			continue ;
		}
		if (page==MenuRandomMapCode){
			ResetGame();
			map.m=rand2(2, 7);
			map.n=rand2(map.m, 17);
			map.nn=rand2(map.n, map.n+6);
			GenerateRandomMap(&map);
			page=MenuPreviewMapCode;
			continue ;
		}
		if (page==MenuCustomMapCode){
			ResetGame();
			page=CustomGameMenu(window, renderer, &map, font36);
			continue ;
		}
		if (page==MenuStartGameCode){
			// printf("before start game\n");
			ResetGame();
			page=MainGameProcess(window, renderer, &map, colormixer, username);
			continue ;
		}
		if (page==MenuPreviewMapCode){
			page=PreviewMapMenu(window, renderer, &map, font36, colormixer);
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
