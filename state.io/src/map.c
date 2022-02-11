#include "main.h"


void FreeMap(struct GameMap *map){
	if (map->pos){
		free(map->pos);
		map->pos = NULL;
	}
	if (map->states){
		free(map->states);
		map->states = NULL;
	}
}

void GenerateRandomMap(struct GameMap *map){
	FreeMap(map);
	n=map->n;
	m=map->m;
	int nn=map->nn, tries=0;
	map->pos=(int*)malloc(nn*sizeof(int));
	// if (map->pos == NULL) error("can't malloc memory :(");
	for (int i=0; i<nn; i++){
		int x=rand2(MinStateDistance/2, Width-MinStateDistance/2);
		int y=rand2(MinStateDistance/2, Height-MinStateDistance/2);
		int bad=0;
		for (int j=0; j<i && !bad; j++){
			int xx=(map->pos[j])>>16, yy=(map->pos[j])&((1<<16)-1);
			if (collide(x-xx, y-yy, MinStateDistance))
				bad=1;
		}
		if (bad){
			tries++;
			if (tries>=200) error("can't generate map with these limits");
			i--;
			continue ;
		}
		map->pos[i]=(x<<16|y);
	}
	map->states=(struct State *)malloc(sizeof(struct State)*map->n);
	// if (map->states == NULL) error("can't malloc memory :(");
	memset(map->states, 0, n*sizeof(struct State));
	for (int i=0; i<n; i++)
		map->states[i].cnt=InitialSoldierCount;
	for (int i=0; i<m; i++)
		map->states[i].owner=i+1;
	
}

void SaveMap(struct GameMap *map, char *filename){
	FILE *f=fopen(filename, "wb");
	fwrite(&(map->n), 4, 1, f);
	fwrite(&(map->nn), 4, 1, f);
	fwrite(&(map->m), 4, 1, f);
	fwrite(map->pos, 4, map->nn, f);
	fclose(f);
}

void LoadMap(struct GameMap *map, char *filename){
	FILE *f=fopen(filename, "rb");
	if (f==NULL) error("map file not found :(");
	
	FreeMap(map);

	fread(&(map->n), 4, 1, f); n=map->n;
	fread(&(map->nn), 4, 1, f);
	fread(&(map->m), 4, 1, f); m=map->m;
	map->pos=(int*)malloc(sizeof(int)*map->nn);
	// if (map->pos == NULL) error("can't malloc memory :(");
	fread(map->pos, 4, map->nn, f);
	fclose(f);
	

	map->states=(struct State *)malloc(sizeof(struct State)*map->n);
	// if (map->states == NULL) error("can't malloc memory :(");
	memset(map->states, 0, map->n*sizeof(struct State));
	for (int i=0; i<map->n; i++)
		map->states[i].cnt=InitialSoldierCount;
	for (int i=0; i<map->m; i++)
		map->states[i].owner=i+1;

}



