# Solarium

**High-performance C++ celestial mechanics and interactive Solar System simulation engine.**

Solarium is an experimental physics, rendering, and simulation project built from first principles in modern C++.

The project started as a headless two-body orbital simulation and is evolving into an interactive Solar System environment focused on celestial mechanics, numerical simulation, real-time visualization, and high-performance scientific computing.

The long-term goal is to create a highly detailed and interactive simulation of our Solar System, combining physically based orbital mechanics with increasingly advanced real-time graphics.

## Current Version

**v0.4.0 — Interactive Orbital Simulation**

### Current Capabilities

* C++20
* CMake-based build system
* 3D vector mathematics
* Newtonian gravitational acceleration
* Numerical orbital integration
* Two-body orbital simulation
* Sun-Earth simulation
* Celestial body representation
* Real-time orbital visualization
* Interactive simulation
* Orbital trajectory visualization
* Camera-based scene exploration
* Physics and simulation architecture
* Basic physics testing

## Architecture

```text
Solarium
│
├── Math
│   └── Vec3
│
├── Physics
│   ├── Gravity
│   ├── Integration
│   └── Orbital Mechanics
│
├── Celestial
│   ├── CelestialBody
│   └── Solar System
│
├── Rendering
│   ├── Scene
│   ├── Camera
│   └── Orbital Visualization
│
├── Simulation
│   ├── Simulation Loop
│   ├── Time Management
│   └── State Updates
│
└── Application
    └── Solarium
```

## Development Roadmap

Solarium is being developed incrementally, with each release expanding both the physics engine and interactive simulation capabilities.

### v0.4.x

* Interactive orbital simulation
* Improved visualization
* Orbital trajectories
* Camera controls
* Simulation controls

### Future

* N-body gravitational simulation
* More accurate orbital mechanics
* Planetary and lunar systems
* Asteroid and comet simulation
* Spacecraft simulation
* Advanced numerical integration
* High-precision astronomical coordinates
* Large-scale spatial simulation
* Procedural planetary terrain
* Atmospheric rendering
* Realistic lighting and shadows
* Advanced planetary visualization
* Performance optimization and parallel simulation

## Design Philosophy

Solarium is being built around a few core principles:

**Physics first**
Build the underlying mechanics from mathematical and physical principles rather than treating the simulation as purely visual.

**Performance**
Use modern C++ and efficient numerical techniques to support increasingly complex simulations.

**Modularity**
Keep mathematics, physics, celestial objects, simulation, and rendering separated so the engine can evolve without becoming tightly coupled.

**Exploration**
Turn the underlying physics into an interactive environment where users can observe and explore celestial systems.

## Status

Solarium is an **experimental project under active development**.

The current release represents an early stage of a much larger vision. The architecture and APIs may change significantly as the simulation engine grows.

## License

See the `LICENSE` file for licensing information.

---

**Solarium - From orbital mechanics to an interactive universe.**
