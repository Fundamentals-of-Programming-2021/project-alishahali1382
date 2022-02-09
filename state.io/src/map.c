#include "main.h"

int distance_state(struct State *A, struct State *B){ // squared distance
	int dx=(A->x)-(B->x), dy=(A->y)-(B->y);
	return dx*dx+dy*dy;
}

void GenerateRandomMap(struct GameMap *map){
	n=map->n;
	m=map->m;
	int nn=map->nn;
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
			i--;
			continue ;
		}
		map->pos[i]=(x<<16|y);
	}
}

void FreeMap(struct GameMap *map){
	free(map->pos);
	free(map->states);
}

