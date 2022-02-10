#include "main.h"

void SaveGame(struct GameMap *map, char username[32], char *filename){
	FILE *f=fopen(filename, "wb");
	// username:
	fwrite(username, 1, 32, f);
	// save map:
	fwrite(&(map->n), 4, 1, f);
	fwrite(&(map->nn), 4, 1, f);
	fwrite(&(map->m), 4, 1, f);
	fwrite(map->pos, 4, map->nn, f);
	// save soldiers:
	fwrite(&cnttroops, 4, 1, f);
	// for (int i=0; i<cnttroops; i++){
	// 	struct Troop *T=troops+i;
	// 	fwrite(&(T->f), 8, 1, f);
	// 	fwrite(&(T->owner), 4, 1, f);
	// 	fwrite(&(T->), 4, 1, f);
	// }
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
	
	// save map:
	tmp=0;
	tmp+=fread(&(map->n), 4, 1, f);
	tmp+=fread(&(map->nn), 4, 1, f);
	tmp+=fread(&(map->m), 4, 1, f);
	if (tmp<3) error("invalid save file");
	
	if (map->pos) free(map->pos);
	map->pos=(int*)malloc(sizeof(int)*map->nn);
	tmp=fread(map->pos, 4, map->nn, f);
	if (tmp<map->nn) error("invalid save file");
	
	// save soldiers:
	tmp=fread(&cnttroops, 4, 1, f);
	if (tmp<1) error("invalid save file");
	
	tmp=fread(troops, sizeof(struct Troop), cnttroops, f);
	if (tmp<cnttroops) error("invalid save file");
	
	tmp=0;
	tmp+=fread(attackqueries, sizeof(struct AttackQuery), MAXATTACKQUERIES, f);
	tmp+=fread(potions, sizeof(struct Potion), MAXPOTIONS, f);
	if (tmp<MAXATTACKQUERIES+MAXPOTIONS) error("invalid save file");
	
	// printf("loaded game\n");
	// printf("username=%s\n", username);
	// printf("n=%d nn=%d m=%d\n", map->n, map->nn, map->m);
	// printf("cnttroops=%d\n", cnttroops);
	// printf("potion-size=%d\n", sizeof(struct Potion));
	// printf("rect-size=%d\n", sizeof(SDL_Rect));

	fclose(f);
}

