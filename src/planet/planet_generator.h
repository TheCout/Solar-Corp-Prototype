#ifndef PLANET_GENERATOR_H
#define PLANET_GENERATOR_H

#include "../general_math/math_types.h"

typedef struct {
    double seed;
    double radius0;
    double radius1;

    int lods[4];
} planet_generator;

// Returns vertex count
int planet_generate_2dmesh(
    planet_generator body,
    int lod,
    double anglea,
    double angleb,
    vec2d* vertices,
    vec2d* uvs,
    vec2d* origin_pos);

// Generates a section of a planet's terrain as a 2d mesh
// Angle B must be larger than Angle A
// Returns vertex count
int planet_generate_2dterrain(
    planet_generator body,
    double anglea,
    double angleb,
    vec2d* vertices,
    vec2d* origin_pos);

int planet_generate_launchpad(
    planet_generator body,
    double angle,
    Vector2* vertices,
    vec2d* origin_pos);

#endif
