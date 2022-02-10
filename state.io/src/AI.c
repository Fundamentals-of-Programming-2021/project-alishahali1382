#include "main.h"

void AI(struct State *states, int dt){
    const int lim = 7;
    int Mx1=-1, id1=-1;
    int cnt=0;
	int tedad[m+1];
	memset(tedad, 0, sizeof(tedad));
	for(int i=0; i<n; i++){
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
    for(int i=0; i<n; i++){
        int now=states[i].cnt-states[i].inq;
        if(Id1==-1 || Mn1>now){
            Mn1=now;
            Id1=i;
        }
    }

	cnt=0;
	for(int i=1; i<=m; i++) cnt+=(tedad[i]>0);
	if (cnt<=1) return ; // game is over!

	int mark = 0;
	for(int i=2; i<=m; i++){
		if (tedad[i]<=15) continue ;
		for(int j=0; j<n; j++){
			if(states[j].cnt-states[j].inq>tedad[i]+4*lim || states[j].owner==i) continue ;
			for(int I=0; I<n; I++){
				if(states[I].owner==i){
					mark=1;
					AddAttackQuery(states, I, j);
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
		if (states[i].owner==ID && now>=lim)
			AddAttackQuery(states, i, Id1);
	}
	// int hala = rand2(0, n);
	// if(hala != 0) return ;
	
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




