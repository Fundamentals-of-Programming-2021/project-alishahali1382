#include "main.h"

const int PotionResolution=60;
const int PotionSpawnDelay=20;

struct Potion potions[MAXPOTIONS];
SDL_Texture *potion_textures[9];
int potionconfig_t[9], potionconfig_writepotiontyp;
double potionconfig_x, potionconfig_y, potionconfig_z;

int active_potion[10];

void ReadPotions(char *filename, SDL_Renderer *renderer){
	FILE *f=fopen(filename, "r");
	if (f==NULL) error("potion-config file not found :(");
	for (int i=1; i<=8; i++)
		fscanf(f, "%d", potionconfig_t+i);
	
	fscanf(f, "%lf", &potionconfig_x);
	fscanf(f, "%lf", &potionconfig_y);
	fscanf(f, "%lf", &potionconfig_z);
	fscanf(f, "%d", &potionconfig_writepotiontyp);
	fclose(f);
	
	for (int i=1; i<=8; i++){
		char S[]="assets/images/potion0.bmp";
		S[20]+=i;
		SDL_Surface *image=SDL_LoadBMP(S);
		if (!image){
			char error_message[60];
			sprintf(error_message, "can't load image file %s :(", S);
			// todo: un-comment
			error(error_message);
			continue ;
		}
		potion_textures[i]=SDL_CreateTextureFromSurface(renderer, image);
		SDL_FreeSurface(image);
	}
}

void UpdatePotions(int dt){
	for (int i=0; i<MAXPOTIONS; i++){
		if (!potions[i].typ) continue ;
		if (!potions[i].owner) continue ;
		potions[i].timer-=dt;
		if (potions[i].timer<=0){
			active_potion[potions[i].owner]=0;
			potions[i].timer=0;
			potions[i].owner=0;
			potions[i].typ=0;
		}
	}
	for (int i=0; i<cnttroops; i++){
		int owner=troops[i].owner;
		if (active_potion[owner]) continue ;
		for (int j=0; j<MAXPOTIONS; j++){
			if (potions[j].owner || !potions[j].typ) continue ;
			if (!IsPointInRect(potions[j].rect, troops[i].x, troops[i].y)) continue ;
			active_potion[owner]=potions[j].typ;
			potions[j].owner=owner;
			potions[j].timer=potionconfig_t[potions[j].typ];
		}
	}
}

void GenerateRandomPotion(struct State *states){
	int id=-1;
	for (int i=0; i<MAXPOTIONS && id==-1; i++)
		if (!potions[i].typ) id=i;

	if (id==-1) return ; // too many potions!

	int x, y, val=-1, ted=3; // try a random location ted times and take the best!
	while (ted--){
		int w1=rand()%6+1, w2=rand()%6+1;
		int i=rand()%n, j=rand()%n;
		if (i==j){
			ted++;
			continue ;
		}
		int xx=(states[i].x*w1 + states[j].x*w2)/(w1+w2);
		int yy=(states[i].y*w1 + states[j].y*w2)/(w1+w2);
		int mn=1e9;
		for (int i=0; i<n; i++)
			mn=min(mn, distance2(xx, yy, states[i].x, states[i].y));
		for (int i=0; i<MAXPOTIONS; i++) if (potions[i].typ)
			mn=min(mn, distance2(xx, yy, potions[i].x, potions[i].y));
		
		if (mn>val){
			val=mn;
			x=xx;
			y=yy;
		}
	}
	potions[id].x=x;
	potions[id].y=y;
	// todo: fix this shit
	while (1){
		potions[id].typ=rand2(1, 9);
		if (potions[id].typ!=2) break ;
	}
	potions[id].rect.w=potions[id].rect.h=PotionResolution;
	potions[id].rect.x=x-PotionResolution/2;
	potions[id].rect.y=y-PotionResolution/2;
}

