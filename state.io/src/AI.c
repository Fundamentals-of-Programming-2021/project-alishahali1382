#include "main.h"

const int AIAttackCoolDown=4000; // how long should a state wait before another attack

int timer[60];
int attack_player1_timer=0;

void ResetAI(){
	memset(timer, 0, sizeof(timer));
	attack_player1_timer=0;
}

void AI(struct State *states, int dt){
	attack_player1_timer+=dt;
	for (int i=0; i<n; i++) timer[i]+=dt;

	
	for (int i=0; i<n; i++) if (timer[i]>=AIAttackCoolDown){
		if (rand()%5!=0 || !states[i].owner || !states[i].cnt || active_potion[states[i].owner]) continue ;
		// go for potion:
		int arr[n], sz=0;
		for (int j=0; j<n; j++) if (j!=i){
			int good=0;
			for (int id=0; id<MAXPOTIONS; id++){
				if (!potions[id].typ || potions[id].owner) continue ;
				int x=states[i].x, y=states[i].y;
				int xx=states[j].x, yy=states[j].y;
				if (SDL_IntersectRectAndLine(&potions[id].rect, &x, &y, &xx, &yy))
					good=1;
			}
			if (good) arr[sz++]=j;
		}
		if (!sz) continue ;
		AddAttackQuery(states, i, arr[rand()%sz]);
		timer[i]=0;
	}

	const int lim = 7;
	int Mx1=-1, id1=-1;
	int cnt=0;
	int tedad[m+1];
	memset(tedad, 0, sizeof(tedad));
	


	for (int i=0; i<n; i++){
		int now=states[i].cnt-states[i].inq;
		tedad[states[i].owner]+=states[i].cnt-states[i].inq;

		if (states[i].owner<=1) continue;
		cnt++;
		if (now<lim) continue;
		if (id1==-1 || Mx1<now){
			Mx1=now;
			id1=i;
		}
	}
	if(id1==-1 || !cnt) return;
	
	int Mn1=1e9, Id1=-1;
	for (int i=0; i<n; i++){
		int now=states[i].cnt-states[i].inq;
		if (Id1==-1 || Mn1>now){
			Mn1=now;
			Id1=i;
		}
	}

	cnt=0;
	for (int i=1; i<=m; i++) cnt+=(tedad[i]>0);
	if (cnt<=1) return ; // game is over!

	int mark = 0;
	for(int i=2; i<=m; i++){
		if (tedad[i]<=15) continue ;
		for(int j=0; j<n; j++){
			if(states[j].cnt-states[j].inq>tedad[i]+4*lim || states[j].owner==i) continue ;
			for(int I=0; I<n; I++){
				if(states[I].owner==i){
					mark=1;
					if (timer[I]>=AIAttackCoolDown){
						AddAttackQuery(states, I, j);
						timer[I]=0;
					}
				}
			}
		}
	}
	if (mark) return ;
	
	int MAX=-1, ID=-1;
	for(int i=2; i<=m; i++){
		if (tedad[i]>MAX){
			ID=i;
			MAX=tedad[i];
		}
	}
	if (ID==-1) return;
	
	int nemone=-1;
	for(int i=0; i<n; i++){
		int now=states[i].cnt-states[i].inq;
		if (states[i].owner==ID && now>=lim){
			if (timer[i]<AIAttackCoolDown) continue ;
			AddAttackQuery(states, i, Id1);
			timer[i]=0;
		}
	}
	
	// attack player 1 :evil_face:
	if (attack_player1_timer<5000) return ;
	attack_player1_timer-=5000;
	
	int f=0;
	for (int i=0; i<n; i++){
		f|=(states[i].owner>1?1:0);
		f|=(states[i].owner==0?2:0);
	}
	if (f<3) return ;

	int u=rand2(0, n), v=rand2(0, n);
	while (states[u].owner<=1) u=rand2(0, n);
	while(states[v].owner!=0) v=rand2(0, n);
	if (states[u].cnt-states[u].inq > states[v].cnt-states[v].inq+6)
		AddAttackQuery(states, u, v);
	return ;
}




