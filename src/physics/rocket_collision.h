#ifndef ROCKET_COLLISION_H
#define ROCKET_COLLISION_H

#include "custom_manifold.h"
#include "../planet/planet_generator.h"
#include "../general_math/math_types.h"

#define POLYGON_MAX_VERTICES 8

typedef struct polygon {
    Vector2 vertices[POLYGON_MAX_VERTICES];
    int vertex_count;
} polygon;

typedef struct rocket_body {
    polygon* polygons;
	int polygon_count;

	float mass;
	float rotation;
	float angular_velocity;
	vec2d local_position;
	vec2d local_velocity;

	// Reserved fields
	float _inv_mass;
	float _inv_inertia;
} rocket_body;

typedef struct collision_result {
    int collided;

    rocket_body* body_a;
    rocket_body* body_b;

    Vector2 body_a_push;
    Vector2 body_b_push;
} collision_result;

// Checks for collisions between the set of rockets in the same sphere of influence (SOI)
// Collisions are either between two rockets or between a rocket and the terrain
// Using the SAT algorithm
// returns rocket collision count
int rockets_check_collision(
    planet_generator planet_body,
    rocket_body* rockets_in_sphere,
    int rocket_count,
    collision_result* results);

#endif
