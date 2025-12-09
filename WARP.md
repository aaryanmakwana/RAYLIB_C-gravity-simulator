# WARP.md

This file provides guidance to WARP (warp.dev) when working with code in this repository.

## Project Overview
A gravity simulator built with C and raylib that simulates gravitational interactions between particles. Uses Newtonian gravity formula: F = G * (m1 * m2) / d²

## Build System
This project uses **CMake** (minimum version 3.15) with C99 standard.

### Building
```bash
# Configure the build
cmake -B build

# Compile
cmake --build build

# Run the executable
./build/bin/gravity_sim
```

### Clean build
```bash
rm -rf build
cmake -B build
cmake --build build
```

## Dependencies
- **raylib 5.0**: Graphics library (auto-fetched from GitHub if not found locally)
- Platform-specific frameworks linked automatically via CMake:
  - macOS: IOKit, Cocoa, OpenGL
  - Linux: m, pthread, dl, rt
  - Windows: winmm, gdi32

## Architecture

### Code Structure
The codebase is minimal with a single-file architecture pattern:
- `main.c`: Entry point, initialization, and render loop
- `lib/includes.h`: Single include file that imports `mylib.h`
- `lib/mylib.h`: All simulation logic (header-only library)

### Key Components

#### Data Structures
- **`struct Body`**: Represents physical properties (mass, location[2], velocity[2])
- **`struct particles`**: Linked list node containing Body, ID, and pointer to next particle

#### Simulation Flow
1. **Initialization** (`CreateParticles(int x)`):
   - Allocates linked list of particles with random properties
   - Mass: 5-20 units, positions: within window bounds, velocities: -10 to 10
   - Returns head of linked list

2. **Render Loop** (in `main.c`):
   - `Gravitate(p)`: Calculate gravitational forces and update velocities/positions (currently unimplemented)
   - `DisplayParticles(p)`: Render particles as circles with radius = mass
   - Fixed 60 FPS

3. **Display** (`DisplayParticles`):
   - Green border around black canvas
   - Particles rendered as green circles scaled by mass

#### Constants (in `mylib.h`)
- `W`, `H`: Window dimensions (1000x1000)
- `G`: Gravitational constant (1)
- `dt`: Time step (0.0166666, approximately 1/60 for 60 FPS)

### Known Issues
- `Gravitate()` function is empty - gravity simulation not implemented
- `GravitateOld()` exists but has bugs and is unused (incorrect force calculations, hardcoded array size)
- Invalid C syntax on lines 48-49 of `mylib.h`: `public` keyword and misplaced variable declarations

### Development Notes
- All simulation code is in header files (no separate .c files for library)
- Linked list structure allows dynamic particle count but current implementation has memory management issues (no cleanup)
- The simulation creates a window with raylib and runs until closed by user
