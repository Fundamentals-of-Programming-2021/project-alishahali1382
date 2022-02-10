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
	
	fclose(f);
}

void LoadGame(struct GameMap *map, char username[32], char *filename){
	map->states=0;
	FILE *f=fopen(filename, "rb");
	if (!f) error("can't open save game file");
	
	// username:
	int tmp=fread(username, 1, 32, f);
	if (tmp<32) error("invalid save file");
	
	// load map:
	tmp=0;
	tmp+=fread(&(map->n), 4, 1, f);
	tmp+=fread(&(map->nn), 4, 1, f);
	tmp+=fread(&(map->m), 4, 1, f);
	if (tmp<3) error("invalid save file");
	
	if (map->pos) free(map->pos);
	map->pos=(int*)malloc(sizeof(int)*map->nn);
	tmp=fread(map->pos, 4, map->nn, f);
	if (tmp<map->nn) error("invalid save file");
	

	if (map->states) free(map->states);
	map->states = (struct State *) malloc(sizeof(struct State)*map->n);
	tmp=fread(map->states, sizeof(struct State), map->n, f);
	if (tmp < map->n) error("invalid save file");


	// load soldiers:
	tmp=fread(&cnttroops, 4, 1, f);
	if (tmp<1 || cnttroops<0 || cnttroops>MAXTROOPS) error("invalid save file");
	
	tmp=fread(troops, sizeof(struct Troop), cnttroops, f);
	if (tmp<cnttroops) error("invalid save file");
	
	// load attack queries and potions
	tmp=0;
	tmp+=fread(attackqueries, sizeof(struct AttackQuery), MAXATTACKQUERIES, f);
	tmp+=fread(potions, sizeof(struct Potion), MAXPOTIONS, f);
	if (tmp<MAXATTACKQUERIES+MAXPOTIONS) error("invalid save file");
	
	// check the save file a little more
	for (int i=0; i<MAXATTACKQUERIES; i++){
		int ok=(attackqueries[i].cnt>=0);
		if (attackqueries[i].cnt==0) continue ;
		ok&=(0<attackqueries[i].owner && attackqueries[i].owner<=map->m);
		ok&=(0<=attackqueries[i].X && attackqueries[i].X<map->n);
		ok&=(0<=attackqueries[i].Y && attackqueries[i].Y<map->n);
		if (!ok) error("invalid save file - invalid attack queries");
	}

	fclose(f);
}

