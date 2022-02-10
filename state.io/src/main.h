#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

typedef long long ll;

#define Width 1080
#define Height 660
#define MAXPOTIONS 5
#define MAXTROOPS 10000
#define MAXATTACKQUERIES 200
extern const int FPS;
extern const int StateRadius;
extern const int MinStateDistance; // maybe change it?
extern const int BorderLineWidth; // the thickness of the lines seperating states
extern const int MinStates, MaxStates;
extern const int MinPlayers, MaxPlayers;
extern const int InitialSoldierCount; // number of soldiers of each state when the game begins
extern const int MaxSoldierCount; // number of generated soldiers of a state during game
extern const int MaxMutualSoldierCount; // number of generated soldiers of a mutual state during game
extern const double SoldierSpeed; // pixel per second
extern const int TroopRadius;
extern const int TroopLinesDistance;
extern const int MaxParallelTroops;
extern const int TroopDelayTime; // how long until the next wave of troops get deployed
extern const double TroopPerSecond; // number of soldiers generated in normal state per second
extern const int PotionResolution;
extern const int PotionSpawnDelay;
extern const int AutoSaveTime; // every AutoSaveTime miliseconds the game is saved

int swap(int *x, int *y);
int min(int x, int y);
int max(int x, int y);
double dmin(double x, double y);
double dmax(double x, double y);
void assert(int f);
int rand2(int l, int r);
int sq(int x);
int distance2(int x, int y, int xx, int yy);
int collide(int dx, int dy, int r);
void error(const char *error_message);
int rgb_to_int(int r, int g, int b);
int IsPointInRect(SDL_Rect rect, int x, int y);
void SortStrings(char S[][50], int n);

extern int n, m;

struct State{
	int x, y; // center position
	int owner, cnt; // cnt: number of troops
	int inq; // number of troops waiting to deploy
	double cnt2; // fractional soldier count
};

void ProcessStates(struct State *states, int dt);


struct GameMap{
	int n, nn, m;
	int *pos; // (x<<16 | y)
	struct State *states;
};

void GenerateRandomMap(struct GameMap *map);
void FreeMap(struct GameMap *map);
void SaveMap(struct GameMap *map, char *filename);
void LoadMap(struct GameMap *map, char *filename);

void SaveGame(struct GameMap *map, char username[32], char *filename);
void LoadGame(struct GameMap *map, char username[32], char *filename);


struct ColorMixer{
	int blank, empty_state, border_line;
	int *minC, *maxC, *C;
};

int getpartialcolor(struct ColorMixer *colormixer, struct State *state);

struct ColorMixer* ReadColorConfig(char *filename);


struct Troop{
	double x, y;
	int owner;
	double f; // f=1 => the path is complete
	// struct State *S1, *S2;
	int S1, S2;
};
extern int cnttroops;
extern struct Troop troops[10000];



struct Potion{
	int x, y;
	int typ, owner;
	int timer; // remaining time of the potion
	SDL_Rect rect;
};
extern struct Potion potions[MAXPOTIONS]; // potions on the screen
extern SDL_Texture *potion_textures[9];
extern int potionconfig_t[9], potionconfig_writepotiontyp;
extern double potionconfig_x, potionconfig_y, potionconfig_z;
extern int active_potion[10]; // for players

void ReadPotions(char *filename, SDL_Renderer *renderer);
void UpdatePotions(int dt);
void GenerateRandomPotion(struct State *states);


extern int A[Width][Height];
void PrepareMap(struct GameMap *map);
void DrawBackGround(SDL_Renderer *renderer, struct State *states, struct ColorMixer *colormixer);
void WriteStateCounts(SDL_Renderer *renderer, struct State *states, TTF_Font *font);
void DrawStates(SDL_Renderer *renderer, struct State *states, struct ColorMixer *colormixer);
void DrawTroops(SDL_Renderer *renderer, struct Troop *troops, struct ColorMixer *colormixer);
void DrawPotions(SDL_Renderer *renderer, struct Potion potions[], struct ColorMixer *colormixer, TTF_Font *font);


struct AttackQuery{
	// struct State *X, *Y;
	int X, Y;
	int owner; // who was the owner of X when Query was made
	int cnt; // sent cnt troops from X-->Y
	int timer; // how long to wait before send another wave
};
extern struct AttackQuery attackqueries[200];


void AddAttackQuery(struct State *states, int X, int Y);
void ProcessTroops(struct State *states, int dt);
void ProcessAttackQueries(struct State *states, int dt);
void ProcessStates(struct State *states, int dt);
void AI(struct State *states, int dt);
void ResetAI();
void ResetGame();


extern const int MenuExitCode;
extern const int MenuMainMenuCode;
extern const int MenuContinueGameCode;
extern const int MenuNewGameCode;
extern const int MenuLeaderboardCode;
extern const int MenuCreditCode;
extern const int MenuChooseMapCode;
extern const int MenuRandomMapCode;
extern const int MenuCustomMapCode;
extern const int MenuStartGameCode;
extern const int MenuPreviewMapCode;




int MainGameProcess(SDL_Window *window, SDL_Renderer *renderer, struct GameMap *map, struct ColorMixer *colormixer, char username[32]);


int MainMenu(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font);
int NewGameMenu(SDL_Window *window, SDL_Renderer *renderer, char username[], TTF_Font *font);
int ChooseMapMenu(SDL_Window *window, SDL_Renderer *renderer, struct GameMap *map, TTF_Font *font);
int CustomGameMenu(SDL_Window *window, SDL_Renderer *renderer, struct GameMap *map, TTF_Font *font);
int PreviewMapMenu(SDL_Window *window, SDL_Renderer *renderer, struct GameMap *map, TTF_Font *font, struct ColorMixer *colormixer);


