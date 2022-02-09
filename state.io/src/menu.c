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

const int ButtonW=270, ButtonH=95;
const int ButtonColor=0xff00d0d0;
const int ButtonColorSelected=0xffd00000;

// (cx, cy): center of button
SDL_Rect DrawButtonCenter(SDL_Renderer *renderer, TTF_Font *font, int cx, int cy, char *text){
	// button background
	SDL_Rect rect={cx-ButtonW/2, cy-ButtonH/2, ButtonW, ButtonH};
	SDL_SetRenderDrawColor(renderer, ButtonColor&255, (ButtonColor>>8)&255, (ButtonColor>>16)&255, 255);
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

SDL_Rect DrawButtonRect(SDL_Renderer *renderer, TTF_Font *font, SDL_Rect *rect, char *text, int col){
	// button background
	SDL_SetRenderDrawColor(renderer, col&255, (col>>8)&255, (col>>16)&255, 255);
	SDL_RenderFillRect(renderer, rect);
	
	SDL_Color color={0, 0, 0}; // text color
	SDL_Surface *text_surface=TTF_RenderText_Solid(font, text, color);
	SDL_Texture *text_texture=SDL_CreateTextureFromSurface(renderer, text_surface);
	int w=text_surface->w, h=text_surface->h;

	SDL_Rect dest={rect->x+rect->w/2-w/2, rect->y+rect->h/2-h/2, w, h};
	SDL_RenderCopy(renderer, text_texture, 0, &dest);

	SDL_FreeSurface(text_surface);
	SDL_DestroyTexture(text_texture);
	
	return *rect;
}


int MainMenu(SDL_Window *window, SDL_Renderer *renderer){
	// continue game
	// new game
	// scoreboard
	// credit
	// exit
	TTF_Font *font=TTF_OpenFont("assets/IRNazaninBold.ttf", 36);
	if (!font) error("can't open font IRNazaninBold.ttf");

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	int logo_left=DrawLogo(window, renderer); // x: left-point of the logo

	int cnt=5;
	SDL_Rect button_rect[cnt];
	char button_text[cnt][20];
	int tmp=(Height-cnt*ButtonH)/(cnt+1);
	// printf("tmp=%d\n", tmp);

	strcpy(button_text[0], "Continue Game");
	strcpy(button_text[1], "New Game");
	strcpy(button_text[2], "Leaderboard");
	strcpy(button_text[3], "Credit");
	strcpy(button_text[4], "Exit");

	for (int i=0; i<cnt; i++) 
		button_rect[i]=DrawButtonCenter(renderer, font, logo_left/2, (i+1)*(tmp+ButtonH)-ButtonH/2, button_text[i]);
	SDL_RenderPresent(renderer);


	int x, y, res=0;
	SDL_Event event;
	while (!res){
		if (!SDL_PollEvent(&event)){
			// note: maybe reduce the delay time
			SDL_Delay(50); // reduce CPU-usage while on menu
			continue ;
		}
		if (event.type == SDL_QUIT)
			return MenuExitCode;
		
		// todo: its possible to check both buttondown and buttonup on same button
		if (event.type == SDL_MOUSEBUTTONDOWN){
			SDL_GetMouseState(&x, &y);
			if (IsPointInRect(button_rect[0], x, y)) res = MenuContinueGameCode;
			if (IsPointInRect(button_rect[1], x, y)) res = MenuNewGameCode;
			if (IsPointInRect(button_rect[2], x, y)) res = MenuLeaderboardCode;
			if (IsPointInRect(button_rect[3], x, y)) res = MenuCreditCode;
			if (IsPointInRect(button_rect[4], x, y)) res = MenuExitCode;
		}
		if (event.type == SDL_MOUSEMOTION){
			int xx, yy, f=0;
			SDL_GetMouseState(&xx, &yy);
			for (int i=0; i<cnt; i++){
				int f0=IsPointInRect(button_rect[i], x, y);
				int f1=IsPointInRect(button_rect[i], xx, yy);
				// printf("i=%d f0=%d f1=%d\n", i, f0, f1);
				if (f0==f1) continue ;
				f=1;
				if (!f0 && f1) DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColorSelected);
				if (f0 && !f1) DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColor);
			}
			// printf("f=%d\n\n", f);
			if (f){
				SDL_RenderPresent(renderer);
				SDL_Delay(100);
			}
			x=xx;
			y=yy;
		}
	}

	TTF_CloseFont(font);

	return res;
}



