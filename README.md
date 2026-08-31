# Solarium

High-performance C++ celestial mechanics and Solar System simulation engine.

Solarium is an experimental physics and simulation project built from first principles in modern C++. The project begins with Newtonian gravity and numerical integration and will progressively evolve toward N-body simulation, orbital mechanics, spacecraft simulation, and high-performance scientific computing.

## Current Version

**v0.1.0 — Two-Body Orbital Simulation**

Current capabilities:

- C++20
- CMake
- 3D vector mathematics
- Newtonian gravitational acceleration
- Velocity Verlet integration
- Sun-Earth simulation
- Basic physics tests
- Headless simulation

## Architecture

```text
Solarium
│
├── Math
│   └── Vec3
│
├── Physics
│   ├── Gravity
│   └── Velocity Verlet
│
├── Celestial
│   └── CelestialBody
│
└── Application
    └── Solar System Simulation
