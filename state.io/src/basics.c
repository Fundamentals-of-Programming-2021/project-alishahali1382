#include "main.h"

const int FPS = 30;
const int StateRadius=35;
const int MinStateDistance=120; // maybe change it?
const int BorderLineWidth=2; // the thickness of the lines seperating states
const int MinStates=6, MaxStates=30;
const int MinPlayers=2, MaxPlayers=6;
const int InitialSoldierCount=10; // number of soldiers of each state when the game begins
const int MaxSoldierCount=50; // number of generated soldiers of a state during game
const int MaxMutualSoldierCount=10; // number of generated soldiers of a mutual state during game
const double SoldierSpeed=45; // pixel per second

int swap(int *x, int *y){ *x^=*y, *y^=*x, *x^=*y;}
int min(int x, int y){ return (x<y?x:y);}
int max(int x, int y){ return (x>y?x:y);}
double dmin(double x, double y){ return (x<y?x:y);}
double dmax(double x, double y){ return (x>y?x:y);}
void assert(int f){ f=1/f;}
int rand2(int l, int r){ return rand()%(r-l)+l;} // random number in [l, r)
int sq(int x){ return x*x;}
int distance2(int x, int y, int xx, int yy){ // squared distance
	int dx=x-xx, dy=y-yy;
	return dx*dx+dy*dy;
}
void error(const char *error_message){
	FILE *f=fopen("error-log.txt", "w");
	fprintf(f, "%s\n", error_message);
	fclose(f);
	exit(1);
}
int rgb_to_int(int r, int g, int b){ return 0xff000000+(b<<16)+(g<<8)+(r);}
