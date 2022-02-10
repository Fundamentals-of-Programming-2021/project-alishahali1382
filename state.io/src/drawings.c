#include "main.h"

const int TroopRadius=6;
const int TroopLinesDistance=15;
const int StateRadius=35;
const int BorderLineWidth=2; // the thickness of the lines seperating states


// A[x][y]=-1   : its a border line
// 0<=A[x][y]<n : its a state
// A[x][y]=n    : its a non-playing field
int A[Width][Height], B[Width][Height]; // B: temp array for making A
void PrepareMap(struct GameMap *map){
	if (!n) error("PrepareMap called with n=0");
	int nn=map->nn;
	int X[nn], Y[nn];
	for (int i=0; i<nn; i++){
		X[i]=(map->pos[i])>>16;
		Y[i]=(map->pos[i])^(X[i]<<16);
	}
	for (int x=0; x<Width; x++) for (int y=0; y<Height; y++){
		int bst=-1, val=1e9;
		for (int i=0; i<nn; i++){
			int d=distance2(x, y, X[i], Y[i]);
			if (d<val){
				val=d;
				bst=i;
			}
		}
		if (bst==-1) error("wtf?! PrepareMap::bst == -1");
		assert(bst!=-1);
		B[x][y]=min(n, bst);
	}
	for (int x=0; x<Width; x++) for (int y=0; y<Height; y++){
		if (x<BorderLineWidth || y<BorderLineWidth || Width-BorderLineWidth<=x || Height-BorderLineWidth<=y)
			A[x][y]=-1;
		else{
			A[x][y]=B[x][y];
			for (int i=-BorderLineWidth; i<=BorderLineWidth; i++) for (int j=-BorderLineWidth; j<=BorderLineWidth; j++){
				if (B[x+i][y+j]==B[x][y]) continue ;
				A[x][y]=-1;
				// maybe use different condition here?
			}
		}
	}
	ll sumx[n], sumy[n], ted[n];
	memset(sumx, 0, sizeof(sumx));
	memset(sumy, 0, sizeof(sumy));
	memset(ted, 0, sizeof(ted));
	for (int x=0; x<Width; x++) for (int y=0; y<Height; y++) if (0<=A[x][y] && A[x][y]<n){
		sumx[A[x][y]]+=x;
		sumy[A[x][y]]+=y;
		ted[A[x][y]]++;
	}
	for (int i=0; i<n; i++){
		if (!ted[i]){
			// map->states[i].x=X[i];
			// map->states[i].y=Y[i];
			// continue ;
			printf("\n\n\n");
			printf("i=%d\n", i);
			printf("n=%d\n", n);
			printf("nn=%d\n", nn);
			
			SaveMap(map, "assets/maps/error-map");
			for (int j=0; j<nn; j++) printf("(%d, %d)\n", X[j], Y[j]);
			error("PrepareMap::ted[i] = 0  ?!?!");
		}
		// assert(ted[i]);
		map->states[i].x=sumx[i]/ted[i];
		map->states[i].y=sumy[i]/ted[i];
	}
}

void DrawBackGround(SDL_Renderer *renderer, struct State *states, struct ColorMixer *colormixer){
	SDL_Surface *surface=SDL_CreateRGBSurface(0, Width, Height, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

	int color[n];
	for (int i=0; i<n; i++) color[i]=getpartialcolor(colormixer, states+i);

	int *pixel=surface->pixels;
	for (int y=0; y<Height; y++) for (int x=0; x<Width; x++){
		if (A[x][y]==-1) *pixel=colormixer->border_line;
		else if (A[x][y]==n) *pixel=colormixer->blank;
		else *pixel=color[A[x][y]];
		pixel++;
	}
	SDL_Texture *texture=SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, 0, 0);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void DrawStates(SDL_Renderer *renderer, struct State *states, struct ColorMixer *colormixer){
	for (int i=0; i<n; i++){
		if (!states[i].owner){
			filledCircleColor(renderer, states[i].x, states[i].y, StateRadius, 0xbfffffff);
		}
		else{
			filledCircleColor(renderer, states[i].x, states[i].y, StateRadius, 0x70000000);
			// circleColor(renderer, states[i].x, states[i].y, StateRadius, colormixer->C[states[i].owner]);
			// filledCircleColor(renderer, states[i].x, states[i].y, StateRadius, colormixer->C[states[i].owner]);
			// circleColor(renderer, states[i].x, states[i].y, StateRadius, colormixer->C[states[i].owner]);
		}
	}
}

void WriteStateCounts(SDL_Renderer *renderer, struct State *states, TTF_Font *font){
	for (int i=0; i<n; i++){
		int owner=states[i].owner;
		// char *text=(char*)malloc(5*sizeof(char));
		char text[5]="";
		sprintf(text, "%d", states[i].cnt);
		SDL_Color color={0, 0, 0};
		if (owner) color.r=color.g=color.b=255;
		SDL_Surface *text_surface=TTF_RenderText_Solid(font, text, color);
		SDL_Texture *text_texture=SDL_CreateTextureFromSurface(renderer, text_surface);
		int x=states[i].x, y=states[i].y, w=text_surface->w, h=text_surface->h;
		// SDL_Rect dest={x-w/2, y+StateRadius+8, w, h};
		SDL_Rect dest={x-w/2, y-h/2, w, h};
		SDL_RenderCopy(renderer, text_texture, 0, &dest);

		// free(text);
		SDL_FreeSurface(text_surface);
		SDL_DestroyTexture(text_texture);
	}
}

void DrawTroops(SDL_Renderer *renderer, struct Troop *troops, struct ColorMixer *colormixer){
	for (int i=0; i<cnttroops; i++){
		if (!troops[i].owner) continue ;
		filledCircleColor(renderer, troops[i].x, troops[i].y, TroopRadius, colormixer->C[troops[i].owner]);
	}
}

void DrawPotions(SDL_Renderer *renderer, struct Potion potions[], struct ColorMixer *colormixer, TTF_Font *font){
	for (int i=0; i<MAXPOTIONS; i++){
		if (!potions[i].typ) continue ;
		if (potions[i].owner)
			filledCircleColor(renderer, potions[i].x, potions[i].y, PotionResolution/2+10, colormixer->C[potions[i].owner]-0x80000000);
		
		// SDL_Rect rect={potions[i].x-PotionResolution/2, potions[i].y-PotionResolution/2, PotionResolution, PotionResolution};
		// SDL_RenderCopy(renderer, potion_textures[potions[i].typ], 0, &rect);
		SDL_RenderCopy(renderer, potion_textures[potions[i].typ], 0, &(potions[i].rect));

		if (potionconfig_writepotiontyp){
			char text[2];
			sprintf(text, "%d", potions[i].typ);
			SDL_Color color={0, 0, 0};
			SDL_Surface *text_surface=TTF_RenderText_Solid(font, text, color);
			SDL_Texture *text_texture=SDL_CreateTextureFromSurface(renderer, text_surface);
			int x=potions[i].x, y=potions[i].y, w=text_surface->w, h=text_surface->h;
			// SDL_Rect dest={x-w/2, y+StateRadius+8, w, h};
			SDL_Rect dest={x-w/2, y-h/2, w, h};
			SDL_RenderCopy(renderer, text_texture, 0, &dest);

			SDL_FreeSurface(text_surface);
			SDL_DestroyTexture(text_texture);
		}

	}
}


