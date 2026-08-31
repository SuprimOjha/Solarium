# Solarium

**High-performance C++ celestial mechanics and interactive Solar System simulation engine.**

Solarium is an experimental physics, rendering, and simulation project built from first principles in modern C++.

The project began as a headless two-body orbital simulation and is evolving toward an interactive Solar System simulation platform combining celestial mechanics, numerical integration, real-time rendering, orbital visualization, and high-performance scientific computing.

## Current Version

**v0.4.0 - Interactive Orbital Simulation**

### Current Capabilities

* C++20
* CMake
* 3D vector mathematics
* Physical constants
* Newtonian gravitational acceleration
* Velocity Verlet integration
* N-body solver foundation
* Celestial body representation
* Centralized body registry
* Solar System simulation
* Simulation configuration
* Simulation clock
* Real-time rendering
* Camera system
* Orbit trails
* Trail rendering
* GLSL planet shaders
* GLSL orbit-trail shaders
* Physics tests
* Simulation tests

## Architecture

The current Solarium source architecture is organized into independent modules for mathematics, physics, celestial systems, simulation, and rendering.

```text
Solarium/
│
├── CMakeLists.txt
├── CMakePresets.json
├── LICENSE
├── README.md
├── .gitignore
│
├── app/
│   └── main.cpp
│
├── assets/
│   └── shaders/
│       ├── planet.frag
│       ├── planet.vert
│       ├── trail.frag
│       └── trail.vert
│
├── external/
│   └── glad/
│       ├── include/
│       │   ├── glad/
│       │   │   └── gl.h
│       │   └── KHR/
│       │       └── khrplatform.h
│       │
│       └── src/
│           └── gl.c
│
├── include/
│   └── solarium/
│       │
│       ├── celestial/
│       │   ├── body_registry.hpp
│       │   └── celestial_body.hpp
│       │
│       ├── math/
│       │   ├── constants.hpp
│       │   └── vec3.hpp
│       │
│       ├── physics/
│       │   ├── gravity.hpp
│       │   ├── n_body_solver.hpp
│       │   └── verlet.hpp
│       │
│       ├── rendering/
│       │   ├── camera.hpp
│       │   ├── orbit_trail.hpp
│       │   ├── renderer.hpp
│       │   ├── shader.hpp
│       │   └── trail_renderer.hpp
│       │
│       └── simulation/
│           ├── simulation.hpp
│           ├── simulation_clock.hpp
│           └── simulation_config.hpp
│
├── src/
│   │
│   ├── celestial/
│   │   ├── body_registry.cpp
│   │   └── celestial_body.cpp
│   │
│   ├── math/
│   │   └── vec3.cpp
│   │
│   ├── physics/
│   │   ├── gravity.cpp
│   │   ├── n_body_solver.cpp
│   │   └── verlet.cpp
│   │
│   ├── rendering/
│   │   ├── camera.cpp
│   │   ├── orbit_trail.cpp
│   │   ├── renderer.cpp
│
```
