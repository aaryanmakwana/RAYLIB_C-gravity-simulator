#!/bin/bash

# Configure and build the project
cmake -B build
cmake --build build

# Run the compiled application
./build/bin/gravity_sim
