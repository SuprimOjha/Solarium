cmake_minimum_required(VERSION 3.20)

project(
    Solarium
    VERSION 0.4.0
    DESCRIPTION "C++ celestial mechanics and OpenGL Solar System simulation engine"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----------------------------------------
# OpenGL
# ----------------------------------------

find_package(OpenGL REQUIRED)

# ----------------------------------------
# GLFW
# ----------------------------------------

find_package(glfw3 3.5 REQUIRED)

# ----------------------------------------
# GLAD
# ----------------------------------------

add_library(glad
    external/glad/src/gl.c
)

target_include_directories(
    glad
    PUBLIC
        ${PROJECT_SOURCE_DIR}/external/glad/include
)

# ----------------------------------------
# Solarium Core
# ----------------------------------------

add_library(solarium_core

    # Math
    src/math/vec3.cpp

    # Celestial
    src/celestial/celestial_body.cpp
    src/celestial/body_registry.cpp

    # Physics
    src/physics/gravity.cpp
    src/physics/verlet.cpp
    src/physics/n_body_solver.cpp

    # Simulation
    src/simulation/simulation_clock.cpp
    src/simulation/simulation.cpp
)

target_include_directories(
    solarium_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# ----------------------------------------
# Core Warnings
# ----------------------------------------

if(MSVC)

    target_compile_options(
        solarium_core
        PRIVATE
            /W4
            /permissive-
    )

else()

    target_compile_options(
        solarium_core
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wshadow
    )

endif()

# ----------------------------------------
# Rendering
# ----------------------------------------

add_library(solarium_rendering

    src/rendering/shader.cpp
    src/rendering/camera.cpp
    src/rendering/renderer.cpp

    src/rendering/orbit_trail.cpp
    src/rendering/trail_renderer.cpp
)

target_include_directories(
    solarium_rendering
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/external/glad/include
)

target_link_libraries(
    solarium_rendering
    PUBLIC
        solarium_core
        glad
        glfw
        OpenGL::GL
)

# ----------------------------------------
# Application
# ----------------------------------------

add_executable(
    solarium
    app/main.cpp
)

target_link_libraries(
    solarium
    PRIVATE
        solarium_rendering
)

# ----------------------------------------
# Testing
# ----------------------------------------

enable_testing()

# ----------------------------------------
# Gravity Tests
# ----------------------------------------

add_executable(
    gravity_tests
    tests/physics/gravity_tests.cpp
)

target_link_libraries(
    gravity_tests
    PRIVATE
        solarium_core
)

add_test(
    NAME GravityTests
    COMMAND gravity_tests
)

# ----------------------------------------
# Verlet Tests
# ----------------------------------------

add_executable(
    verlet_tests
    tests/physics/verlet_tests.cpp
)

target_link_libraries(
    verlet_tests
    PRIVATE
        solarium_core
)

add_test(
    NAME VerletTests
    COMMAND verlet_tests
)

# ----------------------------------------
# N-Body Tests
# ----------------------------------------

add_executable(
    n_body_tests
    tests/physics/n_body_tests.cpp
)

target_link_libraries(
    n_body_tests
    PRIVATE
        solarium_core
)

add_test(
    NAME NBodyTests
    COMMAND n_body_tests
)

# ----------------------------------------
# Simulation Tests
# ----------------------------------------

add_executable(
    simulation_tests
    tests/simulation/simulation_tests.cpp
)

target_link_libraries(
    simulation_tests
    PRIVATE
        solarium_core
)

add_test(
    NAME SimulationTests
    COMMAND simulation_tests
)

# ----------------------------------------
# Assets
# ----------------------------------------

add_custom_command(
    TARGET solarium
    POST_BUILD

    COMMAND
        ${CMAKE_COMMAND}
        -E copy_directory

        ${PROJECT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:solarium>/assets
)