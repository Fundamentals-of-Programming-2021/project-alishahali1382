#include "main.h"

const int AutoSaveTime=5000; // every AutoSaveTime miliseconds the game is saved

void SaveGame(struct GameMap *map, char username[32], char *filename){
	FILE *f=fopen(filename, "wb");
	// username:
	fwrite(username, 1, 32, f);
	// save map:
	fwrite(&(map->n), 4, 1, f);
	fwrite(&(map->nn), 4, 1, f);
	fwrite(&(map->m), 4, 1, f);
	fwrite(map->pos, 4, map->nn, f);
	fwrite(map->states, sizeof(struct State), map->n, f);
	// save soldiers:
	fwrite(&cnttroops, 4, 1, f);
	fwrite(troops, sizeof(struct Troop), cnttroops, f);
	fwrite(attackqueries, sizeof(struct AttackQuery), MAXATTACKQUERIES, f);
	fwrite(potions, sizeof(struct Potion), MAXPOTIONS, f);
	fwrite(active_potion, 4, 10, f);

	fclose(f);
}

void LoadGame(struct GameMap *map, char username[32], char *filename){
	map->states=0;
	FILE *f=fopen(filename, "rb");
	if (!f) error("can't open save game file");
	
	FreeMap(map);

	// username:
	fread(username, 1, 32, f);
	
	// load map:
	fread(&(map->n), 4, 1, f);
	fread(&(map->nn), 4, 1, f);
	fread(&(map->m), 4, 1, f);
	
	map->pos=(int*)malloc(sizeof(int)*map->nn);
	fread(map->pos, 4, map->nn, f);
	

	map->states = (struct State *) malloc(sizeof(struct State)*map->n);
	fread(map->states, sizeof(struct State), map->n, f);
	

	// load soldiers:
	fread(&cnttroops, 4, 1, f);
	fread(troops, sizeof(struct Troop), cnttroops, f);
	fread(attackqueries, sizeof(struct AttackQuery), MAXATTACKQUERIES, f);
	fread(potions, sizeof(struct Potion), MAXPOTIONS, f);
	fread(active_potion, 4, 10, f);
	
	fclose(f);
}

void ResetGame(){
	cnttroops=0;
	memset(troops, 0, sizeof(troops));
	memset(attackqueries, 0, sizeof(attackqueries));
	memset(potions, 0, sizeof(potions));
	memset(active_potion, 0, sizeof(active_potion));
	ResetAI();
}
