cmake_minimum_required(VERSION 3.20)

project(
    Solarium
    VERSION 0.4.0
    DESCRIPTION "C++ celestial mechanics and OpenGL Solar System simulation engine"
    LANGUAGES C CXX
)

option(SOLARIUM_ENABLE_LIVE_EPHEMERIS_TESTS "Enable tests that contact JPL Horizons" OFF)
option(SOLARIUM_ENABLE_SPICE "Enable the NAIF CSPICE ephemeris provider" OFF)

if(SOLARIUM_ENABLE_SPICE)
    find_path(SOLARIUM_SPICE_INCLUDE_DIR SpiceUsr.h)
    find_library(SOLARIUM_SPICE_LIBRARY NAMES cspice)
    if(NOT SOLARIUM_SPICE_INCLUDE_DIR OR NOT SOLARIUM_SPICE_LIBRARY)
        message(FATAL_ERROR "SOLARIUM_ENABLE_SPICE requires the NAIF CSPICE toolkit")
    endif()
endif()

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

find_package(CURL QUIET)

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

    # Ephemeris data foundation
    src/ephemeris/body_id.cpp
    src/ephemeris/canonical_state.cpp
    src/ephemeris/curl_http_client.cpp
    src/ephemeris/jpl_horizons_provider.cpp
    src/ephemeris/spice_backend.cpp
    src/ephemeris/spice_kernel_manager.cpp
    src/ephemeris/spice_provider.cpp
    src/ephemeris/state_cache.cpp
    
    # Physics
    src/physics/gravity.cpp
    src/physics/verlet.cpp
    src/physics/n_body_solver.cpp
    src/physics/physics_diagnostics.cpp
    src/physics/rk4.cpp
    src/physics/rk45.cpp
    src/physics/timestep_controller.cpp
    src/physics/adaptive_integrator.cpp
    # Simulation
    src/simulation/simulation_clock.cpp
    src/simulation/simulation.cpp
    # orbital 
    src/orbital/orbital_converter.cpp
src/orbital/kepler_solver.cpp
# reference
src/time/astronomical_time.cpp
src/time/leap_second_provider.cpp
src/time/time_conversion.cpp
src/time/time_scale_converter.cpp
src/reference/frame_transform.cpp
src/reference/state_transform.cpp
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
    src/rendering/orbit_renderer.cpp
    src/rendering/star_field_renderer.cpp
    src/rendering/ring_renderer.cpp
    src/rendering/body_picker.cpp
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
add_executable(
    diagnostics_tests
    tests/physics/diagnostics_tests.cpp
)

target_link_libraries(
    diagnostics_tests
    PRIVATE
        solarium_core
)

add_test(
    NAME DiagnosticsTests
    COMMAND diagnostics_tests
)
add_executable(
    orbital_elements_tests
    tests/orbital/orbital_elements_tests.cpp
)

target_link_libraries(
    orbital_elements_tests
    PRIVATE
        solarium_core
)

add_test(
    NAME OrbitalElementsTests
    COMMAND orbital_elements_tests
)
add_executable(
    rk45_tests
    tests/physics/rk45_tests.cpp
)

target_link_libraries(
    rk45_tests
    PRIVATE
        solarium_core
)

add_test(
    NAME RK45Tests
    COMMAND rk45_tests
)
add_executable(
    rk4_tests
    tests/physics/rk4_tests.cpp
)

target_link_libraries(
    rk4_tests
    PRIVATE
        solarium_core
)

add_test(
    NAME RK4Tests
    COMMAND rk4_tests
)
add_executable(
    timestep_controller_tests
    tests/physics/timestep_controller_tests.cpp
)

target_link_libraries(
    timestep_controller_tests
    PRIVATE
        solarium_core
)

add_test(
    NAME TimestepControllerTests
    COMMAND timestep_controller_tests
)
add_executable(
    adaptive_integrator_tests
    tests/physics/adaptive_integrator_tests.cpp
)

target_link_libraries(
    adaptive_integrator_tests
    PRIVATE
        solarium_core
)

add_test(
    NAME AdaptiveIntegratorTests
    COMMAND adaptive_integrator_tests
)

add_executable(time_conversion_tests
    tests/time/time_conversion_tests.cpp
)

target_link_libraries(
    time_conversion_tests
    PRIVATE solarium_core
)

add_test(
    NAME TimeConversionTests
    COMMAND time_conversion_tests
)

add_executable(
    astronomical_time_tests
    tests/time/astronomical_time_tests.cpp
)

target_link_libraries(
    astronomical_time_tests
    PRIVATE
        solarium_core
)

add_test(
    NAME AstronomicalTimeTests
    COMMAND astronomical_time_tests
)

add_executable(
    time_scale_converter_tests
    tests/time/time_scale_converter_tests.cpp
)

target_link_libraries(
    time_scale_converter_tests
    PRIVATE
        solarium_core
)

add_test(
    NAME TimeScaleConverterTests
    COMMAND time_scale_converter_tests
)


add_executable(state_transform_tests
    tests/reference/state_transform_tests.cpp
)

target_link_libraries(
    state_transform_tests
    PRIVATE solarium_core
)

add_test(
    NAME StateTransformTests
    COMMAND state_transform_tests
)

add_executable(
    camera_tests
    tests/rendering/camera_tests.cpp
)

target_link_libraries(
    camera_tests
    PRIVATE
        solarium_rendering
)

add_test(
    NAME CameraTests
    COMMAND camera_tests
)

add_executable(
    body_picker_tests
    tests/rendering/body_picker_tests.cpp
)

target_link_libraries(
    body_picker_tests
    PRIVATE
        solarium_rendering
)

add_test(
    NAME BodyPickerTests
    COMMAND body_picker_tests
)

add_executable(
    orbit_geometry_tests
    tests/rendering/orbit_geometry_tests.cpp
)

target_link_libraries(
    orbit_geometry_tests
    PRIVATE
        solarium_rendering
)

add_test(
    NAME OrbitGeometryTests
    COMMAND orbit_geometry_tests
)

add_executable(
    ephemeris_tests
    tests/ephemeris/ephemeris_tests.cpp
)

target_link_libraries(
    ephemeris_tests
    PRIVATE
        solarium_core
)

target_compile_definitions(
    ephemeris_tests
    PRIVATE
        SOLARIUM_FIXTURE_DIR="${CMAKE_SOURCE_DIR}/tests/fixtures"
)

add_test(
    NAME EphemerisTests
    COMMAND ephemeris_tests
)

add_executable(
    jpl_horizons_provider_tests
    tests/ephemeris/jpl_horizons_provider_tests.cpp
)

target_link_libraries(
    jpl_horizons_provider_tests
    PRIVATE
        solarium_core
)

target_compile_definitions(
    jpl_horizons_provider_tests
    PRIVATE
        SOLARIUM_FIXTURE_DIR="${CMAKE_SOURCE_DIR}/tests/fixtures"
)

add_test(
    NAME JplHorizonsProviderTests
    COMMAND jpl_horizons_provider_tests
)

add_executable(
    spice_provider_tests
    tests/ephemeris/spice_provider_tests.cpp
)

target_link_libraries(
    spice_provider_tests
    PRIVATE
        solarium_core
)

add_test(
    NAME SpiceProviderTests
    COMMAND spice_provider_tests
)

if(SOLARIUM_ENABLE_LIVE_EPHEMERIS_TESTS)
    if(NOT CURL_FOUND)
        message(FATAL_ERROR "SOLARIUM_ENABLE_LIVE_EPHEMERIS_TESTS requires libcurl")
    endif()

    add_executable(
        jpl_horizons_live_tests
        tests/ephemeris/jpl_horizons_live_tests.cpp
    )

    target_link_libraries(
        jpl_horizons_live_tests
        PRIVATE
            solarium_core
    )

    add_test(
        NAME JplHorizonsLiveTests
        COMMAND jpl_horizons_live_tests
    )
endif()

if(CURL_FOUND)
    target_link_libraries(solarium_core PRIVATE CURL::libcurl)
    target_compile_definitions(solarium_core PRIVATE SOLARIUM_HAS_CURL=1)
endif()

if(SOLARIUM_ENABLE_SPICE)
    target_include_directories(solarium_core PRIVATE ${SOLARIUM_SPICE_INCLUDE_DIR})
    target_link_libraries(solarium_core PRIVATE ${SOLARIUM_SPICE_LIBRARY})
    target_compile_definitions(solarium_core PRIVATE SOLARIUM_HAS_SPICE=1)
endif()