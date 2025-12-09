#include "./lib/includes.h"


int main(){
  struct particles *p = CreateParticles(10);

  InitWindow(W,H,"GRAVITY SIMULATION");
  SetTargetFPS(60);
  
  while (!WindowShouldClose()){
    BeginDrawing();
    ClearBackground(BLACK);
    Gravitate(p);
    updateParticles(p);
    DisplayParticles(p);
    EndDrawing();
  }
  return 0;
}
