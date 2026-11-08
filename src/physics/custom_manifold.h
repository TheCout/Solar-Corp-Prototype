#ifndef CUSTOM_MANIFOLD_H
#define CUSTOM_MANIFOLD_H

#include <raylib.h>
#include <raymath.h>

typedef struct {
    int has_collision;
    Vector2 normal;          // Outward surface normal of the planet terrain
    float depth;             // Distance the rocket vertex sank into the ground
    Vector2 contact_point;    // Exactly where the crash/touch happened
} terrain_manifold;

terrain_manifold manifold_vertex_terrain(Vector2 vertex, Vector2 p1, Vector2 p2);
int manifolds_polygon_to_terrain(
    Vector2* transformed_vertices,
    int vertex_count,
    Vector2* local_terrain_vertices,
    int terrain_segment_count,
    terrain_manifold* out_manifolds);

#endif
