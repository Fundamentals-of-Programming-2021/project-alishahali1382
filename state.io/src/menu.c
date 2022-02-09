#include "main.h"

const int MenuContinueGameCode=1;
const int MenuNewGameCode=2;
const int MenuLeaderboardCode=3;
const int MenuCreditCode=4;
const int MenuExitCode=5;


// todo: resize the logo.bmp file
int DrawLogo(SDL_Window *window, SDL_Renderer *renderer){
	SDL_Surface *logo_surface=SDL_LoadBMP("assets/logo.bmp");
	if (!logo_surface) error("can't open logo.bmp :(");
	SDL_Texture *logo_texture=SDL_CreateTextureFromSurface(renderer, logo_surface);
	int w=logo_surface->w, h=logo_surface->h;
	int y=(Height-h)/2, x=Width-w-y;
	SDL_Rect dest={x, y, w, h};
	SDL_RenderCopy(renderer, logo_texture, 0, &dest);
	SDL_FreeSurface(logo_surface);
	SDL_DestroyTexture(logo_texture);
	return x;
}

const int ButtonW=250, ButtonH=95;

// (cx, cy): center of button
SDL_Rect DrawButton(SDL_Renderer *renderer, TTF_Font *font, int cx, int cy, char *text){
	// button background
	SDL_Rect rect={cx-ButtonW/2, cy-ButtonH/2, ButtonW, ButtonH};
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	SDL_RenderFillRect(renderer, &rect);
	
	SDL_Color color={0, 0, 0};
	SDL_Surface *text_surface=TTF_RenderText_Solid(font, text, color);
	SDL_Texture *text_texture=SDL_CreateTextureFromSurface(renderer, text_surface);
	int w=text_surface->w, h=text_surface->h;

	SDL_Rect dest={cx-w/2, cy-h/2, w, h};
	SDL_RenderCopy(renderer, text_texture, 0, &dest);

	SDL_FreeSurface(text_surface);
	SDL_DestroyTexture(text_texture);
	
	return rect;
}


int MainMenu(SDL_Window *window, SDL_Renderer *renderer){
	// continue game
	// new game
	// scoreboard
	// credit
	// exit
	TTF_Font *font=TTF_OpenFont("assets/IRNazaninBold.ttf", 34);
	if (!font) error("can't open font IRNazaninBold.ttf");

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	int logo_left=DrawLogo(window, renderer); // x: left-point of the logo

	int cnt=5;
	SDL_Rect buttons[cnt];
	int tmp=(Height-cnt*ButtonH)/(cnt+1);

	buttons[0]=DrawButton(renderer, font, logo_left/2, 1*(tmp+ButtonH)-ButtonH/2, "Continue Game");
	buttons[1]=DrawButton(renderer, font, logo_left/2, 2*(tmp+ButtonH)-ButtonH/2, "New Game");
	buttons[2]=DrawButton(renderer, font, logo_left/2, 3*(tmp+ButtonH)-ButtonH/2, "Leaderboard");
	buttons[3]=DrawButton(renderer, font, logo_left/2, 4*(tmp+ButtonH)-ButtonH/2, "Credit");
	buttons[4]=DrawButton(renderer, font, logo_left/2, 5*(tmp+ButtonH)-ButtonH/2, "Exit");
	SDL_RenderPresent(renderer);

	TTF_CloseFont(font);

	SDL_Event event;
	while (1){
		if (!SDL_PollEvent(&event)){
			// note: maybe reduce the delay time
			SDL_Delay(100); // reduce CPU-usage while on menu
			continue ;
		}
		if (event.type == SDL_QUIT)
			return MenuExitCode;
		
		// todo: its possible to check both buttondown and buttonup on same button
		if (event.type == SDL_MOUSEBUTTONDOWN){
			int x, y;
			SDL_GetMouseState(&x, &y);
			if (IsPointInRect(buttons[0], x, y)) return MenuContinueGameCode;
			if (IsPointInRect(buttons[1], x, y)) return MenuNewGameCode;
			if (IsPointInRect(buttons[2], x, y)) return MenuLeaderboardCode;
			if (IsPointInRect(buttons[3], x, y)) return MenuCreditCode;
			if (IsPointInRect(buttons[4], x, y)) return MenuExitCode;
		}
	}
}
