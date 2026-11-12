cmake_minimum_required(VERSION 3.12)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(RaylibProject C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Force CMake to export compile_commands.json for Zed
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -------------------------------------------------------------
# Fetch and Build Raylib Automatically
# -------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.0
)

# Keep builds fast
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

# -------------------------------------------------------------
# Define Your Executable
# -------------------------------------------------------------
# Replace 'main.c' with your actual source files
add_executable(my_game
    main.c
    general_math/math_types.c
    general_math/double_perlin.c
    physics/custom_manifold.c
    physics/rocket_collision.c
    planet/planet_generator.c
    camera/camera.c
    orbit/orbit_renderer.c
    orbit/orbital_mechanics.c
    orbit/state_vector_orbit.c
    orbit/hke-sdg/keplerh.c
    orbit/hke-sdg/polinomios25.c
    orbit/hke-sdg/polinomios25Q.c
)

# Link raylib to your executable
# This automatically handles includes (-I) and library paths (-L)
target_link_libraries(my_game PRIVATE raylib)
