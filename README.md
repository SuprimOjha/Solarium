# Solarium

**High-performance C++ celestial mechanics and interactive Solar System simulation engine.**

Solarium is an experimental physics, rendering, and simulation project built from first principles in modern C++.

The project began as a headless two-body orbital simulation and is evolving toward an interactive Solar System simulation platform combining celestial mechanics, numerical integration, real-time rendering, orbital visualization, and high-performance scientific computing.

## Current Version

**v0.5.0 - Solar System Visualization Foundation**

### Current Capabilities

* C++20.
* CMake
* 3D vector mathematics
* Physical constants
* Newtonian gravitational acceleration
* Velocity Verlet integration
* N-body solver foundation
* Celestial body representation
* Centralized body registry
* Data-driven Sun, planets, and 20 major moons
* Parent-child celestial relationships and parent-centered moon states
* Approximate orbital, rotation, visual, and atmosphere metadata
* Solar System simulation
* Simulation configuration
* Simulation clock
* Real-time rendering
* Camera system
* WASD/QE camera translation, mouse orbit, and scroll zoom
* Orbit trails
* Trail rendering
* Cached smooth orbital curves
* Deterministic GPU star field
* GLSL planet shaders
* GLSL orbit-trail shaders
* Physics tests
* Simulation tests

## Visualization And Accuracy

The physics state remains in SI units (metres, kilograms, seconds). Rendering converts
positions to astronomical-unit coordinates and applies presentation-only body-size
multipliers so small bodies remain visible. These multipliers never modify the
integrator or gravitational calculations.

The registry values are approximate defaults intended for visualization. They are not
NASA/JPL-validated ephemerides and Solarium does not claim scientific accuracy yet.
Epoch, reference-frame, and time-scale aware ephemeris providers can replace the
initial states later without changing renderers. A future provider can implement the
same boundary as an analytical, file-backed, or JPL ephemeris source.

The current interactive controls are:

* Left mouse drag: orbit the camera.
* Scroll: zoom.
* `WASD`: translate horizontally; `Q/E`: move vertically.
* `Space`: pause/resume; `+`/`-`: change simulation speed; `R`: reset; `Esc`: quit.

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
