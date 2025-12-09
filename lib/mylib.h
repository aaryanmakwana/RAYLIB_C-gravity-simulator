#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>

#define W 1000
#define H 1000
#define G 1
#define dt 0.0166666

struct Body{
  double mass;
  double location[2];
  double velocity[2];
  double force[2];
};

struct particles{
  int id;
  struct Body body;
  struct particles *nextParticle;
};

double RandBetween(int a, int b){
  double ans = (double)rand()*(b-a)/(double)RAND_MAX + a;
  return ans;

}

struct particles* CreateParticles(int x){

  srand(time(NULL));
  struct particles *p;
  p = (struct particles *)malloc(sizeof(struct particles));
  struct particles *n;
  n = p;
  for (int i = 0; i < x; i++) {
    n->id = i;
    n->body.mass = RandBetween(5,20);
    n->body.location[0] = RandBetween(0,W);
    n->body.location[1] = RandBetween(0,H);
    n->body.velocity[0] = RandBetween(-10,10);
    n->body.velocity[1] = RandBetween(-10,10);

    n->nextParticle = (struct particles *)malloc(sizeof(struct particles));
    n = n->nextParticle;
  }

  return p;
}


void DisplayParticles(struct particles * p){
  DrawRectangle(0,0,W,H,GREEN);
  DrawRectangle(5,5,W-10,H-10,BLACK);
  while(p->nextParticle) {
    DrawCircle((int)p->body.location[0],(int)p->body.location[1],(int)p->body.mass,GREEN);
    p = p->nextParticle;
  }
  //DrawText("hi", 10, 10, 10, RED);
}


void Gravitate(struct particles * p){
	while(p){
		struct particles* n = p->nextParticle;
		while(n){
			double F_0 = (n->body.mass * G * p->body.mass) / ((n->body.location[0] - p->body.location[0])*(n->body.location[0] - p->body.location[0]));
			p->body.force[0] += F_0;
			n->body.force[0] += F_0;
			double F_1 = (n->body.mass * G * p->body.mass) / ((n->body.location[1] - p->body.location[1])*(n->body.location[1] - p->body.location[1]));
			p->body.force[1] += F_1;
			n->body.force[1] += F_1;

			n = n->nextParticle;
		}
		p = p->nextParticle;
	}
}

void updateParticles(struct particles *p){
	while(p){
		p->body.velocity[0] += dt*(p->body.force[0] / p->body.mass);
		p->body.velocity[1] += dt*(p->body.force[1]/p->body.mass);

		p->body.location[0] += dt*p->body.velocity[0];
		p->body.location[1] += dt*p->body.velocity[1];

		p = p->nextParticle;
	}
}

void GravitateOld(struct particles * p){
  double new_velocity[10][2];
  struct particles * n;
  struct particles * head;
  head = p;
  double a[2];

  while(p->nextParticle) {
    int i=0;
    n = head;
    while(n->nextParticle) {
      if(n->id != p->id){
        a[0] += (n->body.mass * G) / ((n->body.location[0] - p->body.location[0])*(n->body.location[0] - p->body.location[0]));

        a[1] += (n->body.mass * G) / ((n->body.location[1] - p->body.location[1])*(n->body.location[1] - p->body.location[1]));
      }

      n = n->nextParticle;
    }


    new_velocity[i][0] = p->body.velocity[0] + a[0]*dt;
    new_velocity[i][1] = p->body.velocity[1] + a[1]*dt;
    i++;
    p = p->nextParticle;
  }

  n = head;

  while(p->nextParticle) {
    n->body.velocity[0] = new_velocity[n->id][0];
    n->body.velocity[1] = new_velocity[n->id][1];

    n->body.location[0] = n->body.location[0] + n->body.velocity[0]*dt;
    n->body.location[1] = n->body.location[1] + n->body.velocity[1]*dt;

    n = n->nextParticle;


    if (n->body.location[0]>495) {
      n->body.velocity[0] = -n->body.velocity[0];
    }else if(n->body.location[0]<5){
      n->body.velocity[0] = -n->body.velocity[0];
    }

    if(n->body.location[1]>495){
      n->body.velocity[1] = -n->body.velocity[1];
    }else if(n->body.location[1]<5){
      n->body.velocity[1] = -n->body.velocity[1];
    }
  }

}
