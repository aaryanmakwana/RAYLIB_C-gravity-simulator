#include "./lib/includes.h"


int main(){
    struct particles *p = CreateParticles(15);

    InitWindow(W,H,"GRAVITY SIMULATION");
    SetTargetFPS(FPS);

    while (!WindowShouldClose()){


            Gravitate(p);
            updateParticles(p);
            HandleParticleCollisions(p);

            BeginDrawing();
            ClearBackground(BLACK);
            DrawGravitationalFieldLines(p);
            DisplayParticles(p);

            EndDrawing();
    }

    return 0;
}
