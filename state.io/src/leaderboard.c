#include "main.h"

const int LeaderBoardRows=7;

struct User{
	char name[20];
	int score;
};
struct User users[600];

int SwapUser(struct User *X, struct User *Y){
	if (X->score >= Y->score) return 0;
	swap(&(X->score), &(Y->score));
	for (int i=0; i<20; i++){
		char tmp=X->name[i];
		X->name[i]=Y->name[i];
		Y->name[i]=tmp;
	}
	return 1;
}

void ReadUsers(){
	FILE *f=fopen("assets/leaderboard.txt", "r");
	if (f==NULL) error("can't open leaderboard.txt");
	for (int i=0; i<LeaderBoardRows; i++)
		fscanf(f, "%d %s", &users[i].score, users[i].name);	
	fclose(f);
}

void WriteUsers(){
	FILE *f=fopen("assets/leaderboard.txt", "w");
	if (f==NULL) error("can't edit leaderboard.txt");
	for (int i=0; i<LeaderBoardRows; i++)
		fprintf(f, "%d %s\n", users[i].score, users[i].name);	
	fclose(f);
}

void UpdateScore(char username[], int delta){
	if (!delta) return ; // :)
	for (int i=0; i<600 && users[i].name[0]; i++){
		if (strcmp(username, users[i].name)) continue ;
		users[i].score=max(0, users[i].score+delta);
		if (delta>0){
			for (int j=i; j && SwapUser(users+j-1, users+j); j--) ;
		}
		else{
			for (int j=i; j+1<600 && SwapUser(users+j, users+j+1); j++) ;
		}
		WriteUsers();
	}
	error("did you really enter 600 different names?!");
}

void SortUsers(){
	for (int i=1; i<600; i++){
		for (int j=i; j && SwapUser(users+j-1, users+j); j--) ;
	}
}

int WaitOnScreen(int t){
	t+=SDL_GetTicks();
	SDL_Event event;
	while (SDL_GetTicks()<t){
		while (SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT)
				return MenuExitCode;
			
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
				return MenuMainMenuCode;

		}
		SDL_Delay(100);
	}
	return MenuMainMenuCode;
}

int ShowLeaderBoard(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font){
	SDL_Surface *background_surface=SDL_LoadBMP("assets/images/leaderboard.bmp");
	if (!background_surface) error("can't open leaderboard.bmp :(");
	SDL_Texture *background_texture=SDL_CreateTextureFromSurface(renderer, background_surface);
	SDL_RenderCopy(renderer, background_texture, 0, 0);
	SDL_FreeSurface(background_surface);
	SDL_DestroyTexture(background_texture);

	SortUsers();

	for (int i=0; i<LeaderBoardRows; i++){
			SDL_Color color={0, 0, 0};
			{
				SDL_Surface *text_surface=TTF_RenderText_Solid(font, users[i].name, color);
				SDL_Texture *text_texture=SDL_CreateTextureFromSurface(renderer, text_surface);
				int w=text_surface->w, h=text_surface->h;
				
				SDL_Rect dest={250, 115+82*i, w, h};
				SDL_RenderCopy(renderer, text_texture, 0, &dest);

				SDL_FreeSurface(text_surface);
				SDL_DestroyTexture(text_texture);
			}
			{
				char SS[15];
				sprintf(SS, "%d", users[i].score);
				SDL_Surface *text_surface=TTF_RenderText_Solid(font, SS, color);
				SDL_Texture *text_texture=SDL_CreateTextureFromSurface(renderer, text_surface);
				int w=text_surface->w, h=text_surface->h;
				
				SDL_Rect dest={800, 115+82*i, w, h};
				SDL_RenderCopy(renderer, text_texture, 0, &dest);

				SDL_FreeSurface(text_surface);
				SDL_DestroyTexture(text_texture);
			}
	}
	SDL_RenderPresent(renderer);
	return WaitOnScreen(10000000);
}

int ShowCredit(SDL_Window *window, SDL_Renderer *renderer){
	SDL_Surface *background_surface=SDL_LoadBMP("assets/images/too.bmp");
	if (!background_surface) error("can't open too.bmp :(");
	SDL_Texture *background_texture=SDL_CreateTextureFromSurface(renderer, background_surface);
	SDL_RenderCopy(renderer, background_texture, 0, 0);
	SDL_FreeSurface(background_surface);
	SDL_DestroyTexture(background_texture);
	SDL_RenderPresent(renderer);

	return WaitOnScreen(6500);
}



