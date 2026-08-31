cmake_minimum_required(VERSION 3.20)

project(
    Solarium
    VERSION 0.2.0
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
# Rendering
# ----------------------------------------

add_library(solarium_rendering

    src/rendering/shader.cpp
    src/rendering/camera.cpp
    src/rendering/renderer.cpp
)

target_include_directories(
    solarium_rendering
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
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
# Warnings
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