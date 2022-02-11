#include "main.h"

int n, m; // n: number of states    m: number of players


int GameOver(struct State *states, char username[]){
	int f1=0, f2=0;
	for (int i=0; i<cnttroops; i++){
		if (troops[i].owner==1) f1=1;
		else f2=1;
	}
	for (int i=0; i<n; i++){
		if (states[i].owner==1) f1=1;
		else f2=1;
	}
	if (f1 && f2) return 0;
	// f1 = 1: victory
	// f1 = 0: defeat
	if (f1){
		// show smth
		UpdateScore(username, +1);
	}
	else{
		// show smth
		UpdateScore(username, -1);
	}
	return MenuMainMenuCode;
}


int selected_state; // -1: no state is selected

int GameHandleEvents(SDL_Window *window, struct State *states){
	SDL_Event event;
	while (SDL_PollEvent(&event)){
		if (event.type == SDL_QUIT)
			return MenuExitCode;
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
				if (selected_state==-1){
					// todo: in final release, this should be:
					if (states[id].owner!=1) continue ; // ignore selecting mutual sides
					// if (!states[id].owner) continue ; // ignore selecting mutual sides
					selected_state=id;
				}
				else if (selected_state!=id){
					AddAttackQuery(states, selected_state, id);
					selected_state=-1;
				}
				// if selected_state==id: do nothing
			}
			else if (event.button.button == SDL_BUTTON_RIGHT){
				selected_state=-1;
			}
		}
		if (event.type == SDL_KEYDOWN){
			if (event.key.keysym.sym == SDLK_ESCAPE){
				return MenuMainMenuCode;
			}
		}
	}
	return 0;
}

int MainGameProcess(SDL_Window *window, SDL_Renderer *renderer, struct GameMap *map, struct ColorMixer *colormixer, char username[32]){
	TTF_Font *font28=TTF_OpenFont("assets/IRNazaninBold.ttf", 28);
	n=map->n;
	m=map->m;
	PrepareMap(map);
	struct State *states=map->states;
	
	selected_state=-1;
	


	int begining_of_time = SDL_GetTicks();
	int last_tick=begining_of_time;
	int auto_save_timer=0, res=0;
	while (!res){
		int start_ticks = SDL_GetTicks();
		if (res=GameHandleEvents(window, states)){
			break ;
		}

		int dt=SDL_GetTicks()-last_tick;
		last_tick+=dt;
		
		AI(states, dt);
		ProcessAttackQueries(states, dt);
		ProcessTroops(states, dt);
		ProcessStates(states, dt);
		if (rand()%(FPS*PotionSpawnDelay)==0)
			GenerateRandomPotion(states);
		UpdatePotions(dt);

		res=GameOver(states, username);

		if (selected_state!=-1 && states[selected_state].owner!=1) selected_state=-1;

		if ((auto_save_timer+=dt)>=AutoSaveTime){
			auto_save_timer-=AutoSaveTime;
			SaveGame(map, username, "assets/saved-game");
		}


		DrawBackGround(renderer, states, colormixer);
		DrawPotions(renderer, potions, colormixer, font28);
		DrawStates(renderer, states, colormixer);
		DrawTroops(renderer, troops, colormixer);
		WriteStateCounts(renderer, states, font28);
		if (selected_state!=-1){
			int mousex, mousey;
			SDL_GetMouseState(&mousex, &mousey);
			thickLineColor(renderer, states[selected_state].x, states[selected_state].y, mousex, mousey, 10, 0x60ff5049);
		}
		
		char buffer[100];
		sprintf(buffer, "elapsed time: %dms   FPS: %d", start_ticks-begining_of_time, min(FPS, 1000/max(SDL_GetTicks()-start_ticks, 1)));
		stringRGBA(renderer, 5, 5, buffer, 0, 0, 255, 255);
		

		SDL_RenderPresent(renderer);
		
		SDL_Delay(max(1000/FPS-(SDL_GetTicks()-start_ticks), 0));
	}
	SaveGame(map, username, "assets/saved-game");
	
	FreeMap(map);
	TTF_CloseFont(font28);
	return res;
}


