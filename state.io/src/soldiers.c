#include "main.h"

const int MaxParallelTroops=5;
const int TroopDelayTime=900; // how long until the next wave of troops get deployed
const double TroopPerSecond=0.86; // number of soldiers generated in normal state per second


int cnttroops;
struct Troop troops[MAXTROOPS];

void MoveTroop(struct State *states, struct Troop *T, int dt){ // dt: delta-time in miliseconds
	// printf("shit1\n");
	// if speed-potion was active change zarib
	int owner=T->owner;
	double zarib=1;
	if (active_potion[owner]==1) zarib*=potionconfig_x;
	for (int i=0; i<m; i++) if (i!=owner){
		if (active_potion[i]==3) zarib=0;
		if (active_potion[i]==4) zarib*=potionconfig_y;
	}
	// printf("shit2\n");
	double dx=states[T->S2].x - states[T->S1].x;
	// printf("shit3\n");
	double dy=states[T->S2].y - states[T->S1].y;
	double dist=hypot(dx, dy);
	
	double tmp=zarib*SoldierSpeed*dt/dist/1000;
	dx=dx*tmp;
	dy=dy*tmp;

	// printf("%.3f  %.3f  %.3f\n", tmp, dx, dy);

	T->x+=dx;
	T->y+=dy;
	T->f+=tmp;
}

void ApplyTroopArrival(struct State *S, int x){ // a soldier of player x arrived at S
	if (S->owner==x) S->cnt++;
	else if (S->cnt){
		// potion5: 
		if (active_potion[S->owner]==6){
			S->cnt++;
			return ;
		}

		if (S->cnt == S->inq) S->inq--;
		S->cnt--;
		// when you are under attack, you dont re-generate soldiers:
		S->cnt2=0;
	}
	else{
		// note: maybe change here later
		// change owner of S into x
		S->owner=x;
		S->cnt=1;
	}
}

int isTroopArrived(struct State *states, struct Troop *T){
	// todo: tof for this part
	return hypot((T->x)-(states[T->S2].x), (T->y)-(states[T->S2].y))<=StateRadius+TroopRadius;
}

void ProcessTroops(struct State *states, int dt){
	for (int i=0; i<cnttroops; i++){
		MoveTroop(states, troops+i, dt);
		if (troops[i].f>=1 || isTroopArrived(states, troops+i)){
			ApplyTroopArrival(states + troops[i].S2, troops[i].owner);
			troops[i]=troops[--cnttroops];
			i--;
		}
	}
	// todo: this can be done much faster than O(cnttroops^2)
	for (int i=0; i<cnttroops; i++){
		int bad=0;
		for (int j=i+1; j<cnttroops; j++){
			if (troops[i].owner==troops[j].owner) continue ;
			if (!collide(troops[i].x-troops[j].x, troops[i].y-troops[j].y, 2*TroopRadius)) continue ;
			bad=1;
			cnttroops--;
			troops[j]=troops[cnttroops];
			break ;
		}
		if (bad){
			cnttroops--;
			troops[i]=troops[cnttroops];
			i--;
		}
	}
}

void DeployTroop(struct State *states, int X, int Y, int ted){ // sends ted troops from X-->Y
	double x=states[X].x, y=states[X].y;
	// printf("x0=%f   y0=%f\n", x, y);
	double dx=states[Y].x - x, dy=states[Y].y - y;
	double dist=hypot(dx, dy);
	dx=dx/dist, dy=dy/dist;
	x+=dx*StateRadius, y+=dy*StateRadius;
	// printf("x1=%f   y1=%f\n", x, y);
	double ddx=dy, ddy=-dx;
	x-=ddx*TroopLinesDistance*(ted-1)/2;
	y-=ddy*TroopLinesDistance*(ted-1)/2;
	for (int i=0; i<ted; i++){
		troops[cnttroops].S1=X;
		troops[cnttroops].S2=Y;
		troops[cnttroops].f=0;
		troops[cnttroops].owner=states[X].owner;
		troops[cnttroops].x=x;
		troops[cnttroops].y=y;
		cnttroops++;
		x+=ddx*TroopLinesDistance;
		y+=ddy*TroopLinesDistance;
	}
}


struct AttackQuery attackqueries[MAXATTACKQUERIES];

void AddAttackQuery(struct State *states, int X, int Y){
	if (X==Y || !states[X].owner) return ;

	// potion6: enemies cant attack your states:
	if (states[X].owner!=states[Y].owner && active_potion[states[Y].owner]==6) return ;
	
	int ted=(states[X].cnt)-(states[X].inq);
	if (!ted) return ;
	states[X].inq+=ted;
	for (int i=0; i<200; i++) if (!attackqueries[i].cnt){
		attackqueries[i].owner=states[X].owner;
		attackqueries[i].cnt=ted;
		attackqueries[i].X=X;
		attackqueries[i].Y=Y;
		attackqueries[i].timer=0;
		return ;
	}
	// note: maybe save the game first?
	error("too many attack queries!");
}

void ProcessAttackQueries(struct State *states, int dt){
	for (int i=0; i<200; i++){
		struct AttackQuery *Q=attackqueries+i;
		if (!Q->cnt || Q->owner!=states[Q->X].owner){
			Q->cnt=0;
			continue ;
		}
		
		// potion6: enemies cant attack your states:
		if (states[Q->X].owner!=states[Q->Y].owner && active_potion[states[Q->Y].owner]==6) return ;
	

		Q->timer-=dt;
		if (Q->timer > 0) continue ;
		Q->timer+=TroopDelayTime;
		
		Q->cnt=min(Q->cnt, states[Q->X].inq);
		int ted=min(Q->cnt, MaxParallelTroops);
		Q->cnt-=ted;
		states[Q->X].cnt-=ted;
		states[Q->X].inq-=ted;
		DeployTroop(states, Q->X, Q->Y, ted);
	}
}

void ProcessStates(struct State *states, int dt){
	for (int i=0; i<n; i++){
		// if potion was active change zarib:
		double zarib=1;
		if (active_potion[states[i].owner]==8)
			zarib*=potionconfig_z;
		
		states[i].cnt2+=zarib*TroopPerSecond*dt/1000;
		int ted=states[i].cnt2;
		states[i].cnt2-=ted;

		if (active_potion[states[i].owner]==5){
			states[i].cnt+=ted;
			continue ;
		}

		int lim=(states[i].owner?MaxSoldierCount:MaxMutualSoldierCount);
		if (states[i].cnt<lim)
			states[i].cnt=min(lim, states[i].cnt + ted);

	}
}
