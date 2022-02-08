#include "main.h"

const int TroopRadius=6;
const int TroopLinesDistance=15;
const int StateRadius=35;
const int BorderLineWidth=2; // the thickness of the lines seperating states


// A[x][y]=-1   : its a border line
// 0<=A[x][y]<n : its a state
// A[x][y]=n    : its a non-playing field
int A[Width][Height], B[Width][Height]; // B: temp array for making A
void PrepareMap(struct State *states){
	if (!n) error("PrepareMap called with n=0");
	for (int x=0; x<Width; x++) for (int y=0; y<Height; y++){
		int bst=-1, val=1e9;
		for (int i=0; i<nn; i++){
			int d=distance2(x, y, states[i].x, states[i].y);
			if (d<val){
				val=d;
				bst=i;
			}
		}
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
		// assert(ted[i]);
		states[i].x=sumx[i]/ted[i];
		states[i].y=sumy[i]/ted[i];
		states[i].owner=0;
		states[i].cnt=InitialSoldierCount;
		states[i].inq=0;
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

void DrawStates(SDL_Renderer *renderer, struct State *states, struct ColorMixer *colormixer, TTF_Font *font){
	for (int i=0; i<n; i++){
		int owner=states[i].owner;
		if (!owner){
			filledCircleColor(renderer, states[i].x, states[i].y, StateRadius, 0xbfffffff);
		}
		else{
			filledCircleColor(renderer, states[i].x, states[i].y, StateRadius, 0x70000000);
			// circleColor(renderer, states[i].x, states[i].y, StateRadius, colormixer->C[states[i].owner]);
			// filledCircleColor(renderer, states[i].x, states[i].y, StateRadius, colormixer->C[states[i].owner]);
			// circleColor(renderer, states[i].x, states[i].y, StateRadius, colormixer->C[states[i].owner]);
		}
		char *text=(char*)malloc(4*sizeof(char));
		// memset(text, 0, 5);
		sprintf(text, "%d", states[i].cnt); // todo: some fucking bug here
		// if (!i) printf("text=%s   num=%d\n", text, states[i].cnt);
		SDL_Color color={0, 0, 0};
		if (owner) color.r=color.g=color.b=255;
		SDL_Surface *text_surface=TTF_RenderText_Solid(font, text, color);
		SDL_Texture *text_texture=SDL_CreateTextureFromSurface(renderer, text_surface);
		int x=states[i].x, y=states[i].y, w=text_surface->w, h=text_surface->h;
		// SDL_Rect dest={x-w/2, y+StateRadius+8, w, h};
		SDL_Rect dest={x-w/2, y-h/2, w, h};
		SDL_RenderCopy(renderer, text_texture, 0, &dest);

		free(text);
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
