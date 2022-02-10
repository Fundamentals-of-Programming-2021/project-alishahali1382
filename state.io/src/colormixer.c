#include "main.h"

int getpartialcolor(struct ColorMixer *colormixer, struct State *state){
	if (!(state->owner)) return colormixer->empty_state;
	int ted=min(state->cnt, MaxSoldierCount);
	int mn=colormixer->minC[state->owner]-0xff000000;
	int mx=colormixer->maxC[state->owner]-0xff000000;
	int r=((mn&0xff)*(MaxSoldierCount-ted) + (mx&0xff)*ted)/MaxSoldierCount; mn>>=8; mx>>=8;
	int g=((mn&0xff)*(MaxSoldierCount-ted) + (mx&0xff)*ted)/MaxSoldierCount; mn>>=8; mx>>=8;
	int b=((mn&0xff)*(MaxSoldierCount-ted) + (mx&0xff)*ted)/MaxSoldierCount; mn>>=8; mx>>=8;
	// note: maybe just (mn*a+mx*b)/(a+b)?
	return r+(g<<8)+(b<<16)+0xff000000; // note: maybe handle alpha manually?
}

struct ColorMixer* ReadColorConfig(char *filename){
	FILE *f=fopen(filename, "r");
	if (f==NULL) error("color config file not found :(");
	
	struct ColorMixer *res=(struct ColorMixer *) (malloc(sizeof(struct ColorMixer)));
	res->C=(int*)(malloc(sizeof(int)*(MaxPlayers+1)));
	res->minC=(int*)(malloc(sizeof(int)*(MaxPlayers+1)));
	res->maxC=(int*)(malloc(sizeof(int)*(MaxPlayers+1)));
	
	// memory leak doesn't matter here since its called only once
	char S[50];
	int r, g, b;
	int tmp=fscanf(f, "%s %d,%d,%d", S, &r, &g, &b);
	if (tmp<4 || strcmp(S, "blank:")) error("color-config file invalid line 1 blank");
	res->blank=rgb_to_int(r, g, b);

	tmp=fscanf(f, "%s %d,%d,%d", S, &r, &g, &b);
	if (tmp<4 || strcmp(S, "border-line:")) error("color-config file invalid line 2 border-line");
	res->border_line=rgb_to_int(r, g, b);
	
	tmp=fscanf(f, "%s %d,%d,%d", S, &r, &g, &b);
	if (tmp<4 || strcmp(S, "empty-state:")) error("color-config file invalid line 3 empty-state");
	res->empty_state=rgb_to_int(r, g, b);

	char Smin[]="player0-min:", Smax[]="player0-max:", SS[]="player0:";
	char error_message[]="color-config file invalid player 0 color";
	for (int player=1; player<=MaxPlayers; player++){
		// MaxPlayers<=9
		SS[6]++;
		Smin[6]++;
		Smax[6]++;
		error_message[33]++;
		
		tmp=fscanf(f, "%s %d,%d,%d", S, &r, &g, &b);
		if (tmp<4 || strcmp(S, SS)) error(error_message);
		res->C[player]=rgb_to_int(r, g, b);
		
		tmp=fscanf(f, "%s %d,%d,%d", S, &r, &g, &b);
		if (tmp<4 || strcmp(S, Smin)) error(error_message);
		res->minC[player]=rgb_to_int(r, g, b);
		
		tmp=fscanf(f, "%s %d,%d,%d", S, &r, &g, &b);
		if (tmp<4 || strcmp(S, Smax)) error(error_message);
		res->maxC[player]=rgb_to_int(r, g, b);
	}
	fclose(f);
	return res;
}
