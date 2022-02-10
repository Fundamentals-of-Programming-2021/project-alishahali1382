#include "main.h"
#include <dirent.h>

const int MenuExitCode=-1;
const int MenuMainMenuCode=1;
const int MenuContinueGameCode=2;
const int MenuNewGameCode=3;
const int MenuLeaderboardCode=4;
const int MenuCreditCode=5;
const int MenuChooseMapCode=6;
const int MenuRandomMapCode=7;
const int MenuCustomMapCode=8;
const int MenuStartGameCode=9;
const int MenuPreviewMapCode=10;


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


int MainMenu(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font){
	// continue game
	// new game
	// scoreboard
	// credit
	// exit

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


	int x=0, y=0, res=0;
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

	return res;
}


int NewGameMenu(SDL_Window *window, SDL_Renderer *renderer, char username[], TTF_Font *font){
	// username
	// choose map
	// random map
	// custom map
	// back

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	int logo_left=DrawLogo(window, renderer); // x: left-point of the logo

	int cnt=5;
	SDL_Rect button_rect[cnt];
	char button_text[cnt][20];
	int tmp=(Height-cnt*ButtonH)/(cnt+1);
	// printf("tmp=%d\n", tmp);

	int username_sz=strlen(username);
	sprintf(button_text[0], "@%s", username);
	strcpy(button_text[1], "Choose Map");
	strcpy(button_text[2], "Random Map");
	strcpy(button_text[3], "Custom Map");
	strcpy(button_text[4], "Back");
	
	for (int i=0; i<cnt; i++) 
		button_rect[i]=DrawButtonCenter(renderer, font, logo_left/2, (i+1)*(tmp+ButtonH)-ButtonH/2, button_text[i]);
	SDL_RenderPresent(renderer);

	SDL_StartTextInput();
	
	int x=0, y=0, res=0;
	SDL_Event event;
	while (!res){
		int render_flag=0;
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
			if (IsPointInRect(button_rect[1], x, y) && username_sz) res = MenuChooseMapCode;
			if (IsPointInRect(button_rect[2], x, y) && username_sz) res = MenuRandomMapCode;
			if (IsPointInRect(button_rect[3], x, y) && username_sz) res = MenuCustomMapCode;
			if (IsPointInRect(button_rect[4], x, y)) res = MenuMainMenuCode;
		}
		if (event.type == SDL_MOUSEMOTION){
			int xx, yy, f=0;
			SDL_GetMouseState(&xx, &yy);
			for (int i=1; i<cnt; i++){
				int f0=IsPointInRect(button_rect[i], x, y);
				int f1=IsPointInRect(button_rect[i], xx, yy);
				if (f0==f1) continue ;
				f=1;
			}
			x=xx;
			y=yy;
			if (f) render_flag=1;
		}
		if (event.type == SDL_KEYDOWN){
			if (event.key.keysym.sym == SDLK_ESCAPE){
				res = MenuMainMenuCode;
			}
			if (event.key.keysym.sym == SDLK_BACKSPACE){
				// printf("backspace pressed  username_sz=%d\n", username_sz);
				if (username_sz){
					username[--username_sz]=0;
					render_flag=1;
				}
				// printf("username_sz=%d\n\n", username_sz);
			}
		}
		if (event.type == SDL_TEXTINPUT){
			for (int i=0; i<32 && event.text.text[i] && username_sz<14; i++){
				char ch=event.text.text[i];
				if (ch==' ' || ch=='_') ch=='-';
				if (ch=='-' || isalpha(ch) || isdigit(ch)){
					username[username_sz++]=ch;
					render_flag=1;
				}
			}
		}

		if (render_flag){
			sprintf(button_text[0], "@%s", username);
			
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderClear(renderer);
			DrawLogo(window, renderer);
			for (int i=0; i<cnt; i++){
				int f1=IsPointInRect(button_rect[i], x, y);
				if (f1 && i) DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColorSelected);
				else DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColor);
			}
			SDL_RenderPresent(renderer);
			SDL_Delay(100);
			
			// printf("rendered!\n");
			// printf("%s\n\n", button_text[0]);

		}
	}
	SDL_StopTextInput();

	return res;
}


int ChooseMapMenu(SDL_Window *window, SDL_Renderer *renderer, struct GameMap *map, TTF_Font *font){
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	int logo_left=DrawLogo(window, renderer); // x: left-point of the logo

	
	int ted=0;
	char button_text[31][50]; // no more than 30 maps can be loaded
	

	DIR *dd;
	struct dirent *dir;
	dd=opendir("./assets/maps");
	if (!dd) error("can't open directory ./assets/maps");
	while ((dir=readdir(dd))!=NULL && ted<30){
		// printf("shit1\n");
		if (dir->d_name[0]=='.') continue ;
		strcpy(button_text[ted++], dir->d_name);
		// printf("map-file=%s\n", dir->d_name);
	}
	closedir(dd);
	
	SortStrings(button_text, ted);
	strcpy(button_text[ted++], "Back");

	int cnt=min(6, ted);
	SDL_Rect button_rect[cnt];
	int tmp=(Height-cnt*ButtonH)/(cnt+1);
	// printf("tmp=%d\n", tmp);


	for (int i=0; i<cnt; i++) 
		button_rect[i]=DrawButtonCenter(renderer, font, logo_left/2, (i+1)*(tmp+ButtonH)-ButtonH/2, button_text[i]);
	
	SDL_RenderPresent(renderer);

	int x=0, y=0, scroll=0, res=0, dscroll=0;
	SDL_Event event;
	while (!res){
		dscroll=0;
		if (!SDL_PollEvent(&event)){
			// note: maybe reduce the delay time
			SDL_Delay(50); // reduce CPU-usage while on menu
			continue ;
		}
		if (event.type == SDL_QUIT)
			return MenuExitCode;
		
		if (event.type == SDL_MOUSEBUTTONDOWN){
			SDL_GetMouseState(&x, &y);
			for (int i=0; i<cnt; i++) if (IsPointInRect(button_rect[i], x, y)){
				if (scroll+i==ted-1) res=MenuNewGameCode;
				else{
					char S[70];
					sprintf(S, "assets/maps/%s", button_text[scroll+i]);
					LoadMap(map, S);
					memset(troops, 0, sizeof(troops));
					memset(attackqueries, 0, sizeof(attackqueries));
					memset(potions, 0, sizeof(potions));
					cnttroops=0;
					res=MenuStartGameCode;
				}
			}
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
				// if (f1) DrawButtonRect(renderer, font, button_rect+i, button_text[scroll+i], ButtonColorSelected);
				// else DrawButtonRect(renderer, font, button_rect+i, button_text[scroll+i], ButtonColor);
			}
			x=xx;
			y=yy;
			if (f){
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderClear(renderer);
				DrawLogo(window, renderer);
				for (int i=0; i<cnt; i++){
					int f1=IsPointInRect(button_rect[i], x, y);
					if (f1) DrawButtonRect(renderer, font, button_rect+i, button_text[scroll+i], ButtonColorSelected);
					else DrawButtonRect(renderer, font, button_rect+i, button_text[scroll+i], ButtonColor);
				}
				SDL_RenderPresent(renderer);
				SDL_Delay(100);
			}
		}
		if (event.type == SDL_KEYDOWN){
			if (event.key.keysym.sym == SDLK_ESCAPE){
				res = MenuNewGameCode;
			}
			if (event.key.keysym.sym == SDLK_UP){
				dscroll=-1;
			}
			if (event.key.keysym.sym == SDLK_DOWN){
				dscroll=+1;
			}
		}
		if (event.type == SDL_MOUSEWHEEL){
			if (event.wheel.y<0) dscroll=+1;
			if (event.wheel.y>0) dscroll=-1;
		}
		int f=0;
		if (dscroll==+1 && scroll+cnt<ted) scroll++, f=1;
		if (dscroll==-1 && scroll) scroll--, f=1;
		if (f){
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderClear(renderer);
			DrawLogo(window, renderer);
			for (int i=0; i<cnt; i++){
				int f1=IsPointInRect(button_rect[i], x, y);
				if (f1) DrawButtonRect(renderer, font, button_rect+i, button_text[scroll+i], ButtonColorSelected);
				else DrawButtonRect(renderer, font, button_rect+i, button_text[scroll+i], ButtonColor);
			}
			SDL_RenderPresent(renderer);
			SDL_Delay(100);
		}
	}

	return res;
}


int CustomGameMenu(SDL_Window *window, SDL_Renderer *renderer, struct GameMap *map, TTF_Font *font){
	// Start Game
	// states=
	// players=
	// empty states=
	// back

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	int logo_left=DrawLogo(window, renderer); // x: left-point of the logo

	int cnt=5;
	SDL_Rect button_rect[cnt];
	char button_text[cnt][20];
	int tmp=(Height-cnt*ButtonH)/(cnt+1);
	// printf("tmp=%d\n", tmp);
	
	strcpy(button_text[0], "Start Game");
	strcpy(button_text[4], "Back");
	int n=8, m=2, nn=2;
	sprintf(button_text[1], "states= %d", n);
	sprintf(button_text[2], "players= %d", m);
	sprintf(button_text[3], "empty states= %d", nn);
	for (int i=0; i<cnt; i++)
		button_rect[i]=DrawButtonCenter(renderer, font, logo_left/2, (i+1)*(tmp+ButtonH)-ButtonH/2, button_text[i]);
	SDL_RenderPresent(renderer);		// todo: its possible to check both buttondown and buttonup on same button



	int x=0, y=0, res=0, selected_button=0;
	SDL_Event event;
	while (!res){
		if (!SDL_PollEvent(&event)){
			// note: maybe reduce the delay time
			SDL_Delay(50); // reduce CPU-usage while on menu
			continue ;
		}
		if (event.type == SDL_QUIT)
			return MenuExitCode;
		
		if (event.type == SDL_MOUSEBUTTONDOWN){
			int tmp=selected_button;
			selected_button=0;
			SDL_GetMouseState(&x, &y);
			if (IsPointInRect(button_rect[0], x, y)){
				map->n=n;
				map->m=m;
				map->nn=n+nn;
				GenerateRandomMap(map);
				res = MenuPreviewMapCode;
			}
			if (IsPointInRect(button_rect[1], x, y)) selected_button = 1;
			if (IsPointInRect(button_rect[2], x, y)) selected_button = 2;
			if (IsPointInRect(button_rect[3], x, y)) selected_button = 3;
			if (IsPointInRect(button_rect[4], x, y)) res = MenuNewGameCode;

			SDL_GetMouseState(&x, &y);
			if (tmp!=selected_button){
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderClear(renderer);
				DrawLogo(window, renderer);
				for (int i=0; i<cnt; i++){
					int f1=IsPointInRect(button_rect[i], x, y);
					if (i==0 || i==4){
						if (f1) DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColorSelected);
						else DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColor);
					}
					else{
						if (selected_button==i) DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColorSelected);
						else DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColor);
					}
				}
				SDL_RenderPresent(renderer);
				SDL_Delay(50);
			}
		}
		if (event.type == SDL_MOUSEMOTION){
			int xx, yy, f=0;
			SDL_GetMouseState(&xx, &yy);
			for (int i=0; i<cnt; i++) if (i==0 || i==4){
				int f0=IsPointInRect(button_rect[i], x, y);
				int f1=IsPointInRect(button_rect[i], xx, yy);
				if (f0==f1) continue ;
				f=1;
			}
			x=xx;
			y=yy;
			if (f){
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderClear(renderer);
				DrawLogo(window, renderer);
				for (int i=0; i<cnt; i++){
					int f1=IsPointInRect(button_rect[i], x, y);
					if (i==0 || i==4){
						if (f1) DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColorSelected);
						else DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColor);
					}
					else{
						if (selected_button==i) DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColorSelected);
						else DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColor);
					}
				}
				SDL_RenderPresent(renderer);
				SDL_Delay(50);
			}
		}
		int f=0;
		int dscroll=0;
		if (event.type == SDL_KEYDOWN){
			if (event.key.keysym.sym == SDLK_ESCAPE){
				res = MenuNewGameCode;
			}
			if (event.key.keysym.sym == SDLK_UP){
				dscroll=+1;
			}
			if (event.key.keysym.sym == SDLK_DOWN){
				dscroll=-1;
			}
			if (event.key.keysym.sym == SDLK_TAB){
				selected_button=(selected_button+1)%4;
				f=1;
			}
		}
		if (event.type == SDL_MOUSEWHEEL){
			if (event.wheel.y<0) dscroll=-1;
			if (event.wheel.y>0) dscroll=+1;
		}


		if (!selected_button && !f) continue ;
		
		if (selected_button==1){
			// note: maybe remove upper-limit for n
			if (dscroll==+1 && n<16) n++, f=1;
			if (dscroll==-1 && n-1>=m) n--, f=1;
		}
		if (selected_button==2){
			if (dscroll==+1 && m<MaxPlayers && m+1<=n) m++, f=1;
			if (dscroll==-1 && m>2) m--, f=1;
		}
		if (selected_button==3){
			// note: maybe remove upper-limit for nn
			if (dscroll==+1 && nn<6) nn++, f=1;
			if (dscroll==-1 && nn>0) nn--, f=1;
		}

		if (f){
			sprintf(button_text[1], "states= %d", n);
			sprintf(button_text[2], "players= %d", m);
			sprintf(button_text[3], "empty states= %d", nn);

			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderClear(renderer);
			DrawLogo(window, renderer);
			for (int i=0; i<cnt; i++){
				int f1=IsPointInRect(button_rect[i], x, y);
				if (i==0 || i==4){
					if (f1) DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColorSelected);
					else DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColor);
				}
				else{
					if (selected_button==i) DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColorSelected);
					else DrawButtonRect(renderer, font, button_rect+i, button_text[i], ButtonColor);
				}
			}
			SDL_RenderPresent(renderer);
			SDL_Delay(50);
		}

		
	}

	return res;
}


int PreviewMapMenu(SDL_Window *window, SDL_Renderer *renderer, struct GameMap *map, TTF_Font *font, struct ColorMixer *colormixer){
	PrepareMap(map);
	int res=0;
	return MenuStartGameCode;
	// todo
	/*
	while (1){
		DrawBackGround(renderer, map->states, colormixer);
		SDL_Event event;
		while (SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT)
				return MenuExitCode;
			
		}
		
		SDL_RenderPresent(renderer);
	}*/

}


