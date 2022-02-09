#include "main.h"

const int MaxParallelTroops=5;
const int TroopDelayTime=900; // how long until the next wave of troops get deployed
const double TroopPerSecond=0.86; // number of soldiers generated in normal state per second


int cnttroops;
struct Troop troops[10000];

void MoveTroop(struct Troop *T, int dt){ // dt: delta-time in miliseconds
	// if speed-potion was active change zarib
	int owner=T->owner;
	double zarib=1;
	if (active_potion[owner]==1) zarib*=potionconfig_x;
	for (int i=0; i<m; i++) if (i!=owner){
		if (active_potion[i]==3) zarib=0;
		if (active_potion[i]==4) zarib*=potionconfig_y;
	}
	
	double dx=T->S2->x - T->S1->x, dy=T->S2->y - T->S1->y;
	double dist=hypot(dx, dy);
	
	double tmp=zarib*SoldierSpeed*dt/dist/1000;
	dx=dx/dist*SoldierSpeed*dt/1000;
	dy=dy/dist*SoldierSpeed*dt/1000;

	T->x+=dx;
	T->y+=dy;
	T->f+=tmp;
}

void ApplyTroopArrival(struct State *S, int x){ // a soldier of player x arrived at S
	if (S->owner==x) S->cnt++;
	else if (S->cnt){
		// potion5: 
		if (active_potion[S->owner]==5){
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

int isTroopArrived(struct Troop *T){
	// todo: tof for this part
	return hypot((T->x)-(T->S2->x), (T->y)-(T->S2->y))<=StateRadius+TroopRadius;
}

void ProcessTroops(int dt){
	for (int i=0; i<cnttroops; i++){
		MoveTroop(troops+i, dt);
		if (troops[i].f>=1 || isTroopArrived(troops+i)){
			ApplyTroopArrival(troops[i].S2, troops[i].owner);
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

void DeployTroop(struct State *X, struct State *Y, int ted){ // sends ted troops from X-->Y
	double x=X->x, y=X->y;
	// printf("x0=%f   y0=%f\n", x, y);
	double dx=Y->x - x, dy=Y->y - y;
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
		troops[cnttroops].owner=X->owner;
		troops[cnttroops].x=x;
		troops[cnttroops].y=y;
		cnttroops++;
		x+=ddx*TroopLinesDistance;
		y+=ddy*TroopLinesDistance;
	}
}


struct AttackQuery attackqueries[200];

void AddAttackQuery(struct State *X, struct State *Y){
	if (X==Y || !X->owner) return ;

	// potion6: enemies cant attack your states:
	if (X->owner!=Y->owner && active_potion[Y->owner]==6) return ;
	
	int ted=(X->cnt)-(X->inq);
	if (!ted) return ;
	X->inq+=ted;
	for (int i=0; i<200; i++) if (!attackqueries[i].cnt){
		attackqueries[i].owner=X->owner;
		attackqueries[i].cnt=ted;
		attackqueries[i].X=X;
		attackqueries[i].Y=Y;
		attackqueries[i].timer=0;
		return ;
	}
	// note: maybe save the game first?
	error("too many attack queries!");
}

void ProcessAttackQueries(int dt){
	for (int i=0; i<200; i++){
		struct AttackQuery *Q=attackqueries+i;
		if (!Q->cnt || Q->owner!=Q->X->owner){
			Q->cnt=0;
			continue ;
		}
		
		// potion6: enemies cant attack your states:
		if (Q->X->owner!=Q->Y->owner && active_potion[Q->Y->owner]==6) return ;
	

		Q->timer-=dt;
		if (Q->timer > 0) continue ;
		Q->timer+=TroopDelayTime;
		
		Q->cnt=min(Q->cnt, Q->X->inq);
		int ted=min(Q->cnt, MaxParallelTroops);
		Q->cnt-=ted;
		Q->X->cnt-=ted;
		Q->X->inq-=ted;
		DeployTroop(Q->X, Q->Y, ted);
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
