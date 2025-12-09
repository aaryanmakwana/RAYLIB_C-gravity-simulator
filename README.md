# Gravity Simulator with Raylib

A real-time N-body gravity simulation built with C and raylib that visualizes gravitational interactions between multiple particles.

## Overview

This simulator creates a visual representation of gravitational forces acting between multiple particles in 2D space. Each particle has mass, position, velocity, and experiences forces from all other particles according to Newton's law of universal gravitation.

## Features

- **N-body simulation**: Simulates gravitational interactions between 10 particles (configurable)
- **Real-time visualization**: 60 FPS rendering using raylib
- **Dynamic linked list**: Flexible particle management system
- **Newtonian physics**: Accurate force calculations and Euler integration

## Physics

### Gravitational Force

The simulation uses Newton's law of universal gravitation:

$$F = G \frac{m_1 m_2}{d^2}$$

Where:
- `F` = gravitational force
- `G` = gravitational constant (set to 1 for simulation)
- `m₁`, `m₂` = masses of the two particles
- `d` = distance between particles

### Integration Method

The simulation uses **Euler integration** to update particle states:

**Velocity update:**
$$v_{new} = v_{old} + \frac{F}{m} \cdot \Delta t$$

**Position update:**
$$x_{new} = x_{old} + v_{new} \cdot \Delta t$$

Where `Δt = 0.0166666` (approximately 1/60 second for 60 FPS)

### Simulation Loop

1. **Force Calculation** (`Gravitate()`): Calculate gravitational forces between all particle pairs
2. **State Update** (`updateParticles()`): Update velocities and positions based on accumulated forces
3. **Rendering** (`DisplayParticles()`): Draw particles as circles scaled by mass

## Technical Details

### Data Structures

```c
struct Body {
    double mass;         // Particle mass (5-20 units)
    double location[2];  // X, Y coordinates
    double velocity[2];  // X, Y velocity components
    double force[2];     // Accumulated X, Y forces
};

struct particles {
    int id;                      // Unique particle identifier
    struct Body body;            // Physical properties
    struct particles *nextParticle;  // Linked list pointer
};
```

### Key Functions

- **`CreateParticles(int x)`**: Initializes linked list of x particles with randomized properties
- **`Gravitate(struct particles *p)`**: Calculates gravitational forces between all unique particle pairs
- **`updateParticles(struct particles *p)`**: Updates particle velocities and positions using Euler method
- **`DisplayParticles(struct particles *p)`**: Renders all particles on screen
- **`RandBetween(int a, int b)`**: Utility function for random number generation

### Constants

- `W`, `H` = 1000×1000 (window dimensions)
- `G` = 1 (gravitational constant)
- `dt` = 0.0166666 (time step, ~1/60 second)

## Building and Running

### Prerequisites

- CMake 3.15 or higher
- C99-compatible compiler
- raylib 5.0 (automatically fetched by CMake if not found)

### Build Instructions

```bash
# Configure the build
cmake -B build

# Compile
cmake --build build

# Run the executable
./build/bin/gravity_sim
```

### Clean Build

```bash
rm -rf build
cmake -B build
cmake --build build
```

## Project Structure

```
gravity_sim_raylib/
├── CMakeLists.txt          # Build configuration
├── main.c                  # Entry point and main loop
├── lib/
│   ├── includes.h         # Include wrapper
│   └── mylib.h            # Simulation logic (header-only)
└── README.md
```

## Dependencies

- **raylib 5.0**: Cross-platform graphics library
- Platform-specific libraries (linked automatically):
  - macOS: IOKit, Cocoa, OpenGL
  - Linux: m, pthread, dl, rt
  - Windows: winmm, gdi32

## Known Limitations

- Forces calculated separately per axis (may not perfectly represent 2D vector physics)
- No boundary collision detection
- No particle collision handling
- Memory cleanup not implemented for linked list
- Fixed particle count at initialization

## Future Improvements

- [ ] Add boundary collision detection
- [ ] Implement particle merging on collision
- [ ] Add UI controls for particle count and parameters
- [ ] Implement more accurate vector-based force calculations
- [ ] Add memory cleanup/deallocation
- [ ] Support for variable time stepping
- [ ] Implement Runge-Kutta or Verlet integration for better accuracy

## License

MIT License - feel free to use and modify

## Acknowledgments

Built with [raylib](https://www.raylib.com/) - A simple and easy-to-use library to enjoy videogames programming
