#include "main.h"

// int distance_state(struct State *A, struct State *B){ // squared distance
// 	int dx=(A->x)-(B->x), dy=(A->y)-(B->y);
// 	return dx*dx+dy*dy;
// }

void GenerateRandomMap(struct GameMap *map){
	n=map->n;
	m=map->m;
	int nn=map->nn, tries=0;
	map->pos=(int*)malloc(nn*sizeof(int));
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
}

void FreeMap(struct GameMap *map){
	free(map->pos);
	free(map->states);
	map->pos = NULL;
	map->states = NULL;
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
	int tmp=0;
	tmp+=fread(&(map->n), 4, 1, f);
	tmp+=fread(&(map->nn), 4, 1, f);
	tmp+=fread(&(map->m), 4, 1, f);
	if (tmp<3){
		char S[70];
		sprintf(S, "map file %s not valid :(", filename);
		error(S);
	}
	if (map->pos) free(map->pos);
	map->pos=(int*)malloc(sizeof(int)*map->nn);
	tmp=fread(map->pos, 4, map->nn, f);
	if (tmp<map->nn){
		char S[70];
		sprintf(S, "map file %s not valid :(", filename);
		error(S);
	}
	// todo: check distance of states too
	fclose(f);
}



