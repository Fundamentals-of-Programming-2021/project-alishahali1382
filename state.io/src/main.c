#include "main.h"


const int TroopRadius=6;
const int TroopLinesDistance=15;


int n, m, nn; // n: number of states    m: number of players    nn: n+"number of shit states"

struct Troop{
	double x, y;
	int owner;
	double f; // f=1 => the path is complete
	struct State *S1, *S2;
};
int cnttroops;
struct Troop troops[10000];

void MoveTroop(struct Troop *T, int dt){ // dt: delta-time in miliseconds
	double dx=T->S2->x - T->S1->x, dy=T->S2->y - T->S1->y;
	double dist=hypot(dx, dy);
	double tmp=SoldierSpeed*dt/dist/1000;
	dx=dx/dist*SoldierSpeed*dt/1000;
	dy=dy/dist*SoldierSpeed*dt/1000;

	// note: if 2x-speed potion was active: dx*=2, dy*=2
	T->x+=dx;
	T->y+=dy;
	T->f+=tmp;
}

void ApplyTroopArrival(struct State *S, int x){ // a soldier of player x arrived at S
	if (S->owner==x) S->cnt++;
	else if (S->cnt) S->cnt--;
	else{
		// note: maybe change here later
		// change owner of S into x
		S->owner=x;
		S->cnt=1;
	}
}

int isTroopArrived(struct Troop *T){
	return hypot((T->x)-(T->S2->x), (T->y)-(T->S2->y))<=StateRadius+TroopRadius;
}

void ProcessTroops(int dt){
	for (int i=0; i<cnttroops; i++){
		MoveTroop(troops+i, dt);
		if (troops[i].f>=1 || isTroopArrived(troops+i)){
			ApplyTroopArrival(troops[i].S2, troops[i].owner);
			troops[i]=troops[--cnttroops];
			i--;
		}
	}
	// todo: check collision of troops
}


void DeployTroop(struct State *X, struct State *Y, int ted){ // sends ted troops from X-->Y
	double x=X->x, y=X->y;
	// printf("x0=%f   y0=%f\n", x, y);
	double dx=Y->x - x, dy=Y->y - y;
	double dist=hypot(dx, dy);
	dx=dx/dist, dy=dy/dist;
	x+=dx*StateRadius, y+=dy*StateRadius;
	// printf("x1=%f   y1=%f\n", x, y);
	double ddx=dy, ddy=-dx;
	x-=ddx*TroopLinesDistance*(ted-1)/2;
	y-=ddy*TroopLinesDistance*(ted-1)/2;
	for (int i=0; i<ted; i++){
		troops[cnttroops].S1=X;
		troops[cnttroops].S2=Y;
		troops[cnttroops].f=0;
		troops[cnttroops].owner=X->owner;
		troops[cnttroops].x=x;
		troops[cnttroops].y=y;
		cnttroops++;
		x+=ddx*TroopLinesDistance;
		y+=ddy*TroopLinesDistance;
	}
}


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
		char *text=(char*)malloc(5*sizeof(char));
		sprintf(text, "%d", states[i].cnt);
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


const int EXIT = -1;
int handleEvents(){
	SDL_Event event;
	while (SDL_PollEvent(&event)){
		if (event.type == SDL_QUIT)
			return EXIT;
	}
	return 0;
}


int main(){
	srand(time(0));
	struct ColorMixer *colormixer = ReadColorConfig("assets/color-config.txt");
	if (SDL_Init(SDL_INIT_VIDEO)<0) error(SDL_GetError());
	if (TTF_Init()<0) error(SDL_GetError());
	TTF_Font *font28=TTF_OpenFont("assets/IRNazaninBold.ttf", 28);
	SDL_Window* window = SDL_CreateWindow("state.io", 20, 20, Width, Height, SDL_WINDOW_OPENGL);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	n=10;
	nn=16;
	m=3;
	struct State *states=(struct State *)malloc(nn*sizeof(struct State));
	GenerateRandomMap(states);
	PrepareMap(states);
	// printf("prepared map\n");

	for (int i=0; i<m; i++) states[i].owner=i+1, states[i].cnt=InitialSoldierCount;


	// note: for debug
	DeployTroop(states+0, states+1, 5);


	int begining_of_time = SDL_GetTicks();
	int last_tick=SDL_GetTicks();
	while (1){
		int start_ticks = SDL_GetTicks();
		if (handleEvents()==EXIT) break;

		
		int dt=SDL_GetTicks()-last_tick;
		last_tick+=dt;
		
		// printf("dt=%d   cnttroops=%d\n", dt, cnttroops);
		ProcessTroops(dt);


		
		DrawBackGround(renderer, states, colormixer);
		DrawTroops(renderer, troops, colormixer);
		DrawStates(renderer, states, colormixer, font28);
		
		

		char* buffer = malloc(sizeof(char) * 60);
		sprintf(buffer, "elapsed time: %dms   FPS: %d", start_ticks-begining_of_time, min(FPS, 1000/max(SDL_GetTicks()-start_ticks, 1)));
		stringRGBA(renderer, 5, 5, buffer, 0, 0, 255, 255);
		free(buffer);
		
		SDL_RenderPresent(renderer);
		
		SDL_Delay(max(1000/FPS-(SDL_GetTicks()-start_ticks), 0));
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
