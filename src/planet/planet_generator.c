#include <math.h>
#include "planet_generator.h"
#include "stb_perlin.h"
#include "../my_units.h"

float perlin2d(float x, float y) {
    return stb_perlin_noise3(x, y, 0.0f, 0, 0, 0);
}

float get_circular_noise(float radians, float frequency) {
    // Map 1D radians to a 2D circle
    float x = cosf(radians) * frequency;
    float y = sinf(radians) * frequency;
    // Sample the 2D noise (returns -1.0 to 1.0)
    float noise_val = perlin2d(x, y);
    // Normalize the output to the 0.0 - 1.0 range
    return (noise_val * 0.5f) + 0.5f;
}

float get_terrain_noise(float radians) {

    float noise1 = get_circular_noise(radians, 8) * 0.5;
    float noise2 = get_circular_noise(radians, 14) * 0.3;
    float noise3 = get_circular_noise(radians, noise2 * 40) * 0.15;
    float noise4 = get_circular_noise(radians, (noise3+noise2) * 200) * 0.042;
    return noise1 + noise2 + noise3 + noise4;
}

float get_terrain_height(float radians, float radius, float amplitude, int lod) {
    float full_noise = get_terrain_noise(radians);
    switch (lod) {
        case 1:
            full_noise += get_circular_noise(radians, 300) * 0.08;
            break;
        case 0:
            full_noise += get_circular_noise(radians, 300) * 0.08;
            full_noise += get_circular_noise(radians, 2500 * (1.f - full_noise)) * full_noise * 0.0013;
            break;
            }
    return radius + amplitude * full_noise;
}

vec2d get_radial_pos(float radians, float radius) {
    return (vec2d) { cos(radians) * radius, sin(radians) * radius };
}

vec2d get_noise_pos(float radians, float radius, float amplitude, int lod) {
    float distance = get_terrain_height(radians, radius, amplitude, lod);
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
    float angle_dif = angleb - anglea;

    int vertex_count = 200;
    float delta_angle = angle_dif / (vertex_count - 1);

    float mid_angle = (anglea + angle_dif * 0.5f);
    *origin_pos = get_radial_pos(mid_angle, body.radius0);

    vertices[0] = (vec2d) { -origin_pos->x, -origin_pos->y };

    for (int i = 0; i < vertex_count; i++) {
        float radians = anglea + delta_angle * i;
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

    float angle_dif = angleb - anglea;
    int vertex_count = 80;
    int mid_count = vertex_count / 2;
    float delta_angle = angle_dif / (vertex_count - 1);

    float mid_angle = (anglea + angle_dif * 0.5f);

    for (int i = 0; i < vertex_count; i++) {
        float radians = (anglea + delta_angle * i);
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

    *origin_pos = get_radial_pos(angle, body.radius0 + 92000.0f);

    float length = 10000.f;
    float height = 3000.f;
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
