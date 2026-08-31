cmake_minimum_required(VERSION 3.20)

project(
    Solarium
    VERSION 0.1.0
    DESCRIPTION "High-performance C++ celestial mechanics and Solar System simulation engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----------------------------------------
# Library
# ----------------------------------------

add_library(solarium_core

    src/math/vec3.cpp

    src/physics/gravity.cpp
    src/physics/verlet.cpp

    src/celestial/celestial_body.cpp
)

target_include_directories(
    solarium_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# ----------------------------------------
# Compiler warnings
# ----------------------------------------

if (MSVC)

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
# Application
# ----------------------------------------

add_executable(
    solarium
    app/main.cpp
)

target_link_libraries(
    solarium
    PRIVATE
        solarium_core
)

# ----------------------------------------
# Tests
# ----------------------------------------

enable_testing()

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