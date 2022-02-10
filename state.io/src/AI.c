#include "main.h"

int tedad[100];

void AI(struct State *states, int dt){
    const int lim = 7;
    int Mx1 = -1, id1 = -1;
    int cnt = 0;
	for(int i = 0; i < m; i ++) { tedad[i] = 0; }
    for(int i = 0; i < n; i ++){
        int now = states[i].cnt - states[i].inq;
        if(states[i].owner <= 1) continue;
        cnt ++;
		tedad[states[i].owner] += states[i].cnt - states[i].inq;
        if(now < lim) continue;
        if(id1 == -1 || Mx1 < now)
		{
            Mx1 = now;
            id1 = i;
        }
    }
	if(id1 == -1) return;
    if(cnt == 0) return;
    int Mn1 = 1e9, Id1 = -1;
    for(int i = 0; i < n; i ++){
        int now = states[i].cnt - states[i].inq;
        if(Id1 == -1 || Mn1 > now)
        {
            Mn1 = now;
            Id1 = i;
        }
    }
	cnt = 0;
	for(int i = 1; i < m; i ++)
	{
		if(tedad[i]) cnt ++;
	}
	int mark = 0;
	for(int i = 1; i < m; i ++)
	{
		if(tedad[i] > 20)
		{
			for(int j = 0; j < n; j ++)
			{
				if(states[j].cnt - states[j].inq < tedad[i] + 4 * lim && states[j].owner != i)
				{
					for(int I = 0; I < n; I ++)
					{
						if(states[i].owner == i)
						{
							mark = 1;
							AddAttackQuery(states, I, j);
						}
					}
				}
			}
		}
	}
	if(mark) return;
	printf("shit\n");
	if(cnt <= 0)
	{
		int fir = rand2(0, n);
		while(states[fir].owner != 0)
		{
			fir = rand2(0, n);
		}
		AddAttackQuery(states, fir, id1);
		return;
	}
	else
	{
		int MAX = -1, ID = -1;
		for(int i = 0; i < m; i ++)
		{
			if(tedad[i] > MAX)
			{
				ID = i;
				MAX = tedad[i];
			}
		}
		if(ID == -1) return;
		int nemone = -1;
		for(int i = 0; i < n; i ++)
		{
			int now = states[i].cnt - states[i].inq;
			if(states[i].owner == ID && now >= lim)
			{
				AddAttackQuery(states, i, Id1);
			}
		}
	}
	printf("shit2\n");
	int hala = rand2(0, n);
	if(hala != 0) return ;
	
	int fir = rand2(0, n), sec = rand2(0, n);
	while(states[fir].owner <= 1)
	{
		fir = rand2(0, n);
	}
	while(states[sec].owner != 0 || fir == sec)
	{
		sec = rand2(0, n);
	}
	if(states[fir].cnt - states[fir].inq > states[sec].cnt - states[sec].inq + 6) AddAttackQuery(states, fir, sec);
	return;
}




