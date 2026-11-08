#include "custom_manifold.h"

terrain_manifold manifold_vertex_terrain(Vector2 vertex, Vector2 p1, Vector2 p2) {
    terrain_manifold manifold = { .has_collision = 0, .normal = {0}, .depth = 0, .contact_point = {0} };

    // Vector representing the ground segment
    Vector2 segment = Vector2Subtract(p2, p1);
    float segment_length = Vector2Length(segment);
    if (segment_length < 0.0001f) return manifold;

    Vector2 segment_dir = Vector2Scale(segment, 1.0f / segment_length);

    // Project the rocket vertex onto the segment line to find the closest point
    Vector2 v_to_vertex = Vector2Subtract(vertex, p1);
    float t = Vector2DotProduct(v_to_vertex, segment_dir);

    // Clamp t to the bounds of the segment so we handle edges/peaks correctly
    t = fmaxf(0.0f, fminf(segment_length, t));
    Vector2 closest_point_on_segment = Vector2Add(p1, Vector2Scale(segment_dir, t));

    // Get the vector from the terrain surface to the rocket vertex
    Vector2 separation_vec = Vector2Subtract(vertex, closest_point_on_segment);
    float distance = Vector2Length(separation_vec);

    // Determine the outward terrain normal (perpendicular to the ground segment)
    // For a clockwise vertex chain, the outward normal points "up/out"
    Vector2 terrain_normal = Vector2Normalize((Vector2){ -segment.y, segment.x });

    // Check if the rocket vertex is "inside" the terrain
    // If the separation vector aligns opposite to the terrain normal, it is underground
    if (Vector2DotProduct(separation_vec, terrain_normal) < 0.0f) {
        manifold.has_collision = 1;
        manifold.normal = terrain_normal;
        manifold.depth = distance; // How far below the line it is
        manifold.contact_point = closest_point_on_segment;
    }
    return manifold;
}


int manifolds_polygon_to_terrain(
    Vector2* transformed_vertices,
    int vertex_count,
    Vector2* local_terrain_vertices,
    int terrain_segment_count,
    terrain_manifold* out_manifolds) {

    int active_manifold_count = 0;

    // Loop through every vertex
    for (int i = 0; i < vertex_count; i++) {
        Vector2 vertex = transformed_vertices[i];

        terrain_manifold deepest_manifold_for_vertex;
        int vertex_collided = 0;
        float max_depth = -1.0f;

        // Loop only through the few segments directly under/near the polygon
        for (int j = 0; j < terrain_segment_count; j++) {
            Vector2 p1 = local_terrain_vertices[j];
            Vector2 p2 = local_terrain_vertices[(j + 1) % terrain_segment_count]; // Wrap around if needed

            terrain_manifold m = manifold_vertex_terrain(vertex, p1, p2);

            // Keep the collision that has the deepest penetration depth
            if (m.has_collision && m.depth > max_depth) {
                max_depth = m.depth;
                deepest_manifold_for_vertex = m;
                vertex_collided = true;
            }
        }

        // Only add ONE manifold per  vertex
        if (vertex_collided) {
            out_manifolds[active_manifold_count++] = deepest_manifold_for_vertex;
        }
    }
    return active_manifold_count;
}
