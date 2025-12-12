#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include <math.h> // Added for sqrt function

#define W 1000
#define H 700
#define G 10
#define SIM_SPEED 1.0
#define FPS 60
#define dt (SIM_SPEED/FPS)
#define MIN_DIST_SQ 25.0 // Minimum squared distance to prevent extreme gravitational forces (radius 5)
#define COEFFICIENT_OF_RESTITUTION 1.0 // How "bouncy" the collisions are (1.0 for perfectly elastic)
#define FIELD_LINE_SPACING 50 // Distance between field line origins on the grid
#define FIELD_LINE_LENGTH 15 // Length of each field line segment/arrow
#define MIN_FIELD_CALC_DIST_SQ 10.0 // Min squared distance for field line

struct Body{
  double mass;
  double location[2];
  double velocity[2];
  double force[2]; // force[0] for Fx, force[1] for Fy
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

  if (x <= 0) {
      return NULL; // No particles to create
  }

  struct particles *head = (struct particles *)malloc(sizeof(struct particles));
  if (!head) {
      perror("Failed to allocate memory for head particle");
      return NULL;
  }
  struct particles *current = head;

  for (int i = 0; i < x; i++) {
    current->id = i;
    current->body.mass = RandBetween(5,20); // Mass also determines radius for drawing
    current->body.location[0] = RandBetween(0,W);
    current->body.location[1] = RandBetween(0,H);
    current->body.velocity[0] = RandBetween(-100,100);
    current->body.velocity[1] = RandBetween(-100,100);
    current->body.force[0] = 0.0; // Initialize forces to zero
    current->body.force[1] = 0.0;

    if (i < x - 1) { // If not the last particle, allocate next
      current->nextParticle = (struct particles *)malloc(sizeof(struct particles));
      if (!current->nextParticle) {
          perror("Failed to allocate memory for a particle");
          // If allocation fails mid-loop, we should ideally free previously allocated nodes.
          // For simplicity, we'll return the partially created list.
          return head;
      }
      current = current->nextParticle;
    } else { // Last particle, terminate the list
      current->nextParticle = NULL;
    }
  }

  return head;
}


void DisplayParticles(struct particles * p){
  DrawRectangle(0,0,W,H,GREEN); // Outer border
  DrawRectangle(5,5,W-10,H-10,BLACK); // Inner background

  struct particles *current = p; // Use a temporary pointer for traversal
  while(current) { // Iterate through all particles, including the last one
    // Ensure mass is positive for radius, otherwise, default to a small value
    int radius = (int)current->body.mass;
    if (radius <= 0) radius = 1; // Minimum radius for drawing

    DrawCircle((int)current->body.location[0], (int)current->body.location[1], radius, GREEN);
    current = current->nextParticle;
  }
}


void Gravitate(struct particles *head){
    struct particles *p1 = head;

    // 1. Reset all forces to zero at the beginning of each step
    while (p1) {
        p1->body.force[0] = 0.0;
        p1->body.force[1] = 0.0;
        p1 = p1->nextParticle;
    }

    // Reset p1 to head to start force calculation
    p1 = head;

    // 2. Calculate forces between all unique pairs
    while (p1) {
        struct particles *p2 = p1->nextParticle; // Start from the next particle to avoid self-interaction and duplicate pairs
        while (p2) {
            double dx = p2->body.location[0] - p1->body.location[0];
            double dy = p2->body.location[1] - p1->body.location[1];
            double distSq = dx*dx + dy*dy;

            // Prevent division by zero or extreme forces when particles are too close
            // Use a small minimum distance squared (e.g., 5*5 = 25) to prevent instability
            if (distSq < 25.0) {
                distSq = 25.0; // Effectively a minimum distance of 5 pixels
            }

            double dist = sqrt(distSq);
            double forceMagnitude = (G * p1->body.mass * p2->body.mass) / distSq;

            // Calculate force components (Fx, Fy) using vector directions
            double Fx = forceMagnitude * (dx / dist);
            double Fy = forceMagnitude * (dy / dist);

            // Apply forces (action-reaction)
            p1->body.force[0] += Fx;
            p1->body.force[1] += Fy;

            p2->body.force[0] -= Fx; // Force on p2 is equal and opposite to force on p1
            p2->body.force[1] -= Fy;

            p2 = p2->nextParticle;
        }
        p1 = p1->nextParticle;
    }
}

void HandleParticleCollisions(struct particles *head) {
    struct particles *p1 = head;
    while (p1) {
        struct particles *p2 = p1->nextParticle; // Check against all subsequent particles
        while (p2) {
            double r1 = p1->body.mass; // Using mass as radius
            double r2 = p2->body.mass;

            // Ensure radii are positive
            if (r1 <= 0) r1 = 1.0;
            if (r2 <= 0) r2 = 1.0;

            double dx = p2->body.location[0] - p1->body.location[0];
            double dy = p2->body.location[1] - p1->body.location[1];
            double distSq = dx*dx + dy*dy;
            double minCombinedRadius = r1 + r2;
            double minCombinedRadiusSq = minCombinedRadius * minCombinedRadius;

            // Check for collision (overlap)
            if (distSq < minCombinedRadiusSq) {
                // Collision detected!

                double dist = sqrt(distSq);
                // Calculate collision normal (unit vector pointing from p1 to p2)
                double nx = dx / dist;
                double ny = dy / dist;

                // 1. Resolve Overlap (push particles apart)
                // This prevents "sticking" and repeated collisions in the same frame
                double overlap = minCombinedRadius - dist;
                p1->body.location[0] -= nx * overlap * 0.5;
                p1->body.location[1] -= ny * overlap * 0.5;
                p2->body.location[0] += nx * overlap * 0.5;
                p2->body.location[1] += ny * overlap * 0.5;


                // 2. Calculate relative velocity
                double vRelX = p1->body.velocity[0] - p2->body.velocity[0];
                double vRelY = p1->body.velocity[1] - p2->body.velocity[1];

                // Relative velocity along the normal (dot product)
                double speedAlongNormal = vRelX * nx + vRelY * ny;

                // Do not resolve if particles are already moving apart
                if (speedAlongNormal > 0) {
                    p2 = p2->nextParticle;
                    continue; // No collision response needed if separating
                }

                // 3. Calculate impulse (for elastic collision)
                double e = COEFFICIENT_OF_RESTITUTION; // Coefficient of restitution
                double impulseScalar = -(1.0 + e) * speedAlongNormal / ((1.0 / p1->body.mass) + (1.0 / p2->body.mass));

                // Apply impulse to change velocities
                p1->body.velocity[0] += impulseScalar * nx / p1->body.mass;
                p1->body.velocity[1] += impulseScalar * ny / p1->body.mass;

                p2->body.velocity[0] -= impulseScalar * nx / p2->body.mass;
                p2->body.velocity[1] -= impulseScalar * ny / p2->body.mass;
            }

            p2 = p2->nextParticle;
        }
        p1 = p1->nextParticle;
    }
}


void updateParticles(struct particles *p){
	while(p){
        // Update velocity based on force and mass (F=ma => a=F/m, v_new = v_old + a*dt)
		p->body.velocity[0] += dt*(p->body.force[0] / p->body.mass);
		p->body.velocity[1] += dt*(p->body.force[1]/p->body.mass);

        // Update location based on velocity (x_new = x_old + v*dt)
		p->body.location[0] += dt*p->body.velocity[0];
		p->body.location[1] += dt*p->body.velocity[1]; // Corrected: should be dt*p->body.velocity[1]


        // Simple boundary collision (bounce off walls)
        // Adjust bounds considering the particle's mass as its radius
        double radius = p->body.mass;
        if (radius <= 0) radius = 1;

        double minX = 5.0 + radius; // Left bound, considering radius and 5px border
        double maxX = W - 5.0 - radius; // Right bound
        double minY = 5.0 + radius; // Top bound
        double maxY = H - 5.0 - radius; // Bottom bound

        if (p->body.location[0] < minX) {
            p->body.location[0] = minX; // Reposition to boundary to prevent sticking
            p->body.velocity[0] *= -1.0; // Reverse velocity
        } else if (p->body.location[0] > maxX) {
            p->body.location[0] = maxX;
            p->body.velocity[0] *= -1.0;
        }

        if (p->body.location[1] < minY) {
            p->body.location[1] = minY;
            p->body.velocity[1] *= -1.0;
        } else if (p->body.location[1] > maxY) {
            p->body.location[1] = maxY;
            p->body.velocity[1] *= -1.0;
        }

		p = p->nextParticle;
	}
}

void DrawGravitationalFieldLines(struct particles *head) {
    if (!head) return; // No particles, no field to draw

    for (int gy = FIELD_LINE_SPACING / 2; gy < H; gy += FIELD_LINE_SPACING) {
        for (int gx = FIELD_LINE_SPACING / 2; gx < W; gx += FIELD_LINE_SPACING) {
            double total_field_x = 0.0;
            double total_field_y = 0.0;

            struct particles *current_particle = head;
            while (current_particle) {
                // Vector from grid point (test mass) to the current_particle's center
                double dx = current_particle->body.location[0] - gx;
                double dy = current_particle->body.location[1] - gy;

                double distSq = dx*dx + dy*dy;

                // Prevent extremely large field vectors when very close to a mass
                if (distSq < MIN_FIELD_CALC_DIST_SQ) {
                    distSq = MIN_FIELD_CALC_DIST_SQ;
                }

                double dist = sqrt(distSq);

                // Calculate the magnitude of the gravitational acceleration (field strength)
                // Field strength = G * M / r^2
                double fieldMagnitude = (G * current_particle->body.mass) / distSq;

                // Add component of the field vector (points towards the mass)
                // Remember: dx/dist and dy/dist are unit vectors from test point to mass
                total_field_x += fieldMagnitude * (dx / dist);
                total_field_y += fieldMagnitude * (dy / dist);

                current_particle = current_particle->nextParticle;
            }

            // Draw the field line if there's a significant field
            double total_field_magnitude = sqrt(total_field_x*total_field_x + total_field_y*total_field_y);
            if (total_field_magnitude > 0.1) { // Only draw if field is strong enough to avoid tiny lines from noise
                // Normalize the total field vector
                printf(".");
                double nx = total_field_x / total_field_magnitude;
                double ny = total_field_y / total_field_magnitude;

                // Define start and end points of the line segment
                Vector2 startPoint = { (float)gx, (float)gy };
                Vector2 endPoint = { (float)(gx + nx * FIELD_LINE_LENGTH), (float)(gy + ny * FIELD_LINE_LENGTH) };

                DrawLineV(startPoint, endPoint, RED); // Draw a red line
            }
        }
    }
}
