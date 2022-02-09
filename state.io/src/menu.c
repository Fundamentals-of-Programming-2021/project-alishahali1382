#include "main.h"

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

// todo: draw buttons too

// continue game
// new game
// scoreboard
// credit


int MainMenu(SDL_Window *window, SDL_Renderer *renderer){
	TTF_Font *font=TTF_OpenFont("assets/IRNazaninBold.ttf", 34);
	if (!font) error("can't open font IRNazaninBold.ttf");

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	int x=DrawLogo(window, renderer); // x: left-point of the logo



	SDL_RenderPresent(renderer);

	SDL_Delay(3000);

}
