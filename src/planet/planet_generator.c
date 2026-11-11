#include <math.h>
#include <stdlib.h>
#include "planet_generator.h"
#include "../general_math/double_perlin.h"

double get_circular_noise_safe(double radians, double frequency) {
    // Calculate the exact 2D circle coordinate using double precision
    double x_exact = cos(radians) * frequency;
    double y_exact = sin(radians) * frequency;
    double noise_val = perlin_noise3d(x_exact, y_exact, 0.0);
    return (noise_val * 0.5) + 0.5;
}

double get_terrain_noise(double radians) {
    double noise1 = get_circular_noise_safe(radians, 10) * 0.6;
    double noise2 = get_circular_noise_safe(radians, 100) * 0.35;
    double noise3 = get_circular_noise_safe(radians, (noise1+noise2) * 350) * 0.022;
    double noise4 = get_circular_noise_safe(radians, (noise3+noise2) * 6000) * 0.0001;
    return noise1 + noise2 + noise3 + noise4;
}

double get_terrain_height(double radians, double radius, float amplitude, int lod) {
    double full_noise = get_terrain_noise(radians);
    switch (lod) {
        case 0:
            radius += get_circular_noise_safe(radians, 10000 * full_noise) * 8;
            break;
            }
    return radius + amplitude * full_noise;
}

vec2d get_radial_pos(double radians, double radius) {
    return (vec2d) { cos(radians) * radius, sin(radians) * radius };
}

vec2d get_noise_pos(double radians, double radius, float amplitude, int lod) {
    double distance = get_terrain_height(radians, radius, amplitude, lod);
    return get_radial_pos(radians, distance);
}

int planet_generate_2dmesh(
    planet_generator body,
    int lod,
    double anglea,
    double angleb,
    vec2d* vertices,
    vec2d* uvs,
    vec2d* origin_pos) {

    float amplitude = body.radius1 - body.radius0;
    double angle_dif = angleb - anglea;

    int vertex_count = 200;
    double delta_angle = angle_dif / (vertex_count - 1);

    double mid_angle = (anglea + angle_dif * 0.5);
    *origin_pos = get_radial_pos(mid_angle, body.radius0);

    vertices[0] = (vec2d) { -origin_pos->x, -origin_pos->y };

    for (int i = 0; i < vertex_count; i++) {
        double radians = anglea + delta_angle * i;
        vec2d pos = get_noise_pos(radians, body.radius0, amplitude, lod);
        vertices[i+1] = (vec2d) { pos.x - origin_pos->x, pos.y - origin_pos->y };
    }
    return vertex_count;
}

// Returns vertex count
int planet_generate_2dterrain(
    planet_generator body,
    double anglea,
    double angleb,
    vec2d* vertices,
    vec2d* origin_pos) {

    float amplitude = body.radius1 - body.radius0;

    double angle_dif = angleb - anglea;
    int vertex_count = 20;
    int mid_count = vertex_count / 2;
    double delta_angle = angle_dif / (vertex_count - 1);

    double mid_angle = (anglea + angle_dif * 0.5f);

    for (int i = 0; i < vertex_count; i++) {
        double radians = (anglea + delta_angle * i);
        vec2d pos = get_noise_pos(radians, body.radius0, amplitude, 0);
        vertices[i] = pos;
        if (i == mid_count) {
            *origin_pos = pos;
        }
    }
    return vertex_count;
}

int planet_generate_launchpad(
    planet_generator body,
    double angle,
    Vector2* vertices,
    vec2d* origin_pos) {

    *origin_pos = get_radial_pos(angle, body.radius0 + 16000.0f);

    float length = 4000.f;
    float height = 500.f;
    float hf_length = length / 2.f;
    float hf_height = height / 2.f;
    vertices[0] = (Vector2) { .x = -hf_length - height, .y = -hf_height };
    vertices[1] = (Vector2) { .x = -hf_length, .y = -hf_height };
    vertices[2] = (Vector2) { .x = -hf_length, .y =  hf_height };

    vertices[3] = (Vector2) { .x = -hf_length, .y = -hf_height };
    vertices[4] = (Vector2) { .x =  hf_length, .y =  hf_height };
    vertices[5] = (Vector2) { .x = -hf_length, .y =  hf_height };

    vertices[6] = (Vector2) { .x = -hf_length, .y = -hf_height };
    vertices[7] = (Vector2) { .x =  hf_length, .y = -hf_height };
    vertices[8] = (Vector2) { .x =  hf_length, .y =  hf_height };

    vertices[9]  = (Vector2) { .x = hf_length, .y = -hf_height };
    vertices[10] = (Vector2) { .x = hf_length + height, .y = -hf_height };
    vertices[11] = (Vector2) { .x = hf_length, .y =  hf_height };
    return 12;
}
