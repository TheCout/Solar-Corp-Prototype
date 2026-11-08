#include <raylib.h>
#include <rlgl.h>
#include <stdio.h>
#include "math_types.h"
#include "my_units.h"
#include "orbit/orbital_mechanics.h"
#include "camera/camera.h"
#include "planet/planet_generator.h"

#define DEFAULT_WIN_WIDTH 1200
#define DEFAULT_WIN_HEIGHT 800

struct planet_descriptor {
    Color color;
    double radius;
    double sphere_of_influence;
    double gravitational_parameter;
};

void draw_terrain(
    vec2d* vertices,
    int vertex_count,
    vec2d origin_pos,
    vec2d cam_pos,
    double zoom,
    Color tint) {
    if (vertex_count < 3) return;
    rlBegin(RL_TRIANGLES); // We use RL_TRIANGLES instead of fan!
    rlColor4ub(tint.r, tint.g, tint.b, tint.a);

    vec2d centerVertex = {
        (int)(600 + (vertices[0].x + origin_pos.x - cam_pos.x) * zoom),
        (int)(400 - (vertices[0].y + origin_pos.y - cam_pos.y) * zoom),
    };

    for (int i = 1; i < vertex_count - 1; i++) {
        rlVertex2f(centerVertex.x, centerVertex.y);

        rlVertex2f(
            (600 + (vertices[i].x + origin_pos.x - cam_pos.x) * zoom),
            (400 - (vertices[i].y + origin_pos.y - cam_pos.y) * zoom)
        );
        rlVertex2f(
            (600 + (vertices[i + 1].x + origin_pos.x - cam_pos.x) * zoom),
            (400 - (vertices[i + 1].y + origin_pos.y - cam_pos.y) * zoom)
        );
    }

    rlEnd();
}

int screen_width, screen_height;
float half_screen_width, half_screen_height;

int main() {
    screen_width = DEFAULT_WIN_WIDTH;
    screen_height = DEFAULT_WIN_HEIGHT;
    half_screen_width = (float)DEFAULT_WIN_WIDTH / 2.0f;
    half_screen_height = (float)DEFAULT_WIN_HEIGHT / 2.0f;

    vec2d planets_positions[7] = {};
    orbit planets_orbits[7] = {};
    orbit_elements planets_orbit_elements[7] = {};
    struct planet_descriptor planets_descriptors[7] = {};
    planet_generator planets_generators[7] = {};

    planets_orbits[0] = (orbit) { // Mercury
        .argument_of_periapsis = 77,
        .eccentricity = 0.2056,
        .semi_major_axis = 0.38709893 * 1.496e+11,
        .period = 0.2,
        .clockwise = 0,
    };
    planets_orbits[1] = (orbit) { // Venus
        .argument_of_periapsis = 131,
        .eccentricity = 0.006,
        .semi_major_axis = 0.723332 * 1.496e+11,
        .period = 0.6,
        .clockwise = 0,
    };
    planets_orbits[2] = (orbit) { // Earth
        .argument_of_periapsis = 102,
        .eccentricity = 0.0167,
        .semi_major_axis = 1.00000011 * 1.496e+11,
        .period = 1,
        .clockwise = 0,
    };
    planets_orbits[3] = (orbit) { // Mars
        .argument_of_periapsis = 336,
        .eccentricity = 0.0934,
        .semi_major_axis = 1.52366231 * 1.496e+11,
        .period = 1.9,
        .clockwise = 0,
    };

    planets_descriptors[0] = (struct planet_descriptor) { // Mercury
        .color = DARKGRAY,
        .radius = 2440530,
        .sphere_of_influence = 112000000,
        .gravitational_parameter = 2.2032e+13,
    };
    planets_descriptors[1] = (struct planet_descriptor) { // Venus
        .color = YELLOW,
        .radius =  6051800,
        .sphere_of_influence = 616000000,
        .gravitational_parameter = 3.24859e+13,
    };
    planets_descriptors[2] = (struct planet_descriptor) { // Earth
        .color = BLUE,
        .radius = 6378137,
        .sphere_of_influence = 924000000,
        .gravitational_parameter = 3.986004418e+14,
    };
    planets_descriptors[3] = (struct planet_descriptor) { // Mars
        .color = RED,
        .radius = 3396200,
        .sphere_of_influence = 577000000,
        .gravitational_parameter = 4.28283752e+13,
    };

    //
    // GENERATES THE PLANETS MESHES AND TERRAIN COLLISIONS
    //
    planets_generators[0] = (planet_generator) { // Mercury
        .radius0 = 2240530,
        .radius1 = 2440530,
        .lods = {10000, 2000, 400, 20},
    };
    planets_generators[1] = (planet_generator) { // Venus
        .radius0 = 5901800,
        .radius1 =  6051800,
        .lods = {10000, 2000, 400, 20},
    };
    planets_generators[2] = (planet_generator) { // Earth
        .radius0 = 6208137,
        .radius1 = 6378137,
        .lods = {50000, 2000, 200, 20},
    };
    planets_generators[3] = (planet_generator) { // Mars
        .radius0 = 3306200,
        .radius1 = 3396200,
        .lods = {10000, 2000, 400, 20},
    };

    struct camera my_camera = camera_default();

    my_camera.origin_position = &planets_positions[2];
    my_camera.transform = (struct transform) {
        .local_position = {6378137, 0}
    };
    my_camera.zoom = 0.0004;
    camera_set_target_zoom(&my_camera, 0.0004);
    camera_update_dimensions(&my_camera, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT);

    TraceLog(LOG_INFO, "Hello world!\n");

    InitWindow(DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT, "SolarCorp 2.0");
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        for (int i = 0; i < 4; i++) {
            planets_orbit_elements[i] = om_orbit_update_elapsed_time(planets_orbits[i], planets_orbit_elements[i]);
            planets_orbit_elements[i].time_since_periapsis += dt;
            planets_positions[i] = om_orbit_get_position(planets_orbits[i], planets_orbit_elements[i]);
        }

        vec2d direction = { 0.0, 0.0 };
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) direction.x += 1.0;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  direction.x -= 1.0;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  direction.y -= 1.0;
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    direction.y += 1.0;

        if (direction.x != 0.0f || direction.y != 0.0f) {
            direction = vec2d_normalized(direction);
            my_camera.transform.local_position.x += direction.x * 1000 / my_camera.zoom * dt;
            my_camera.transform.local_position.y += direction.y * 1000 / my_camera.zoom * dt;
        }

        if (my_camera.origin_position) {
            my_camera.transform.world_position.x = my_camera.origin_position->x + my_camera.transform.local_position.x;
            my_camera.transform.world_position.y = my_camera.origin_position->y + my_camera.transform.local_position.y;
        } else {
            my_camera.transform.world_position.x = my_camera.transform.local_position.x;
            my_camera.transform.world_position.y = my_camera.transform.local_position.y;
        }

        float cam_input_y = GetMouseWheelMove();
        if (cam_input_y != 0) {
            camera_zoom_input(&my_camera, cam_input_y);
        }
        if (IsWindowResized()) {
            screen_width = GetScreenWidth();
            screen_height = GetScreenHeight();
            half_screen_width = (float)screen_width / 2.0f;
            half_screen_height = (float)screen_height / 2.0f;
        }

        camera_update_zoom(&my_camera, dt);
        camera_update_dimensions(&my_camera, screen_width, screen_height);

        int lod = 3;
        if (my_camera.zoom > 0.0025) {
            lod = 0;
        } else if (my_camera.zoom > 0.00075) {
            lod = 1;
        } else if (my_camera.zoom > 0.00003) {
            lod = 2;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        double zoom = half_screen_width / my_camera.width;
        for (int i = 0; i < 4; i++) {
            // Check for each planet if its visible in the camera
            if (camera_circle_to_circle_collision(my_camera, planets_positions[i], planets_generators[i].radius1)) {
                double angle0, angle1;
                if (camera_is_point_in_view(my_camera, planets_positions[i])) {
                    angle0 = 0;
                    angle1 = TWO_PI;
                }  else {
                    vec2d relative_pos = (vec2d) {
                        .x = planets_positions[i].x - my_camera.transform.world_position.x,
                        .y = planets_positions[i].y - my_camera.transform.world_position.y
                    };
                    camera_angular_aperture_from_point(my_camera, relative_pos, &angle0, &angle1);
                }

                vec2d vertices[planets_generators[i].lods[lod] + 1];
                vec2d uvs[planets_generators[i].lods[lod] + 1];
                vec2d origin_pos;
                int vertex_count = planet_generate_2dmesh(planets_generators[i], lod, angle0, angle1, vertices, uvs, &origin_pos);

                draw_terrain(
                    vertices,
                    vertex_count + 1,
                    vec2d_add(origin_pos, planets_positions[i]),
                    my_camera.transform.world_position,
                    zoom,
                    planets_descriptors[i].color);

                DrawCircle(
                    (int)(half_screen_width + (planets_positions[i].x - my_camera.transform.world_position.x) * zoom),
                    (int)(half_screen_height - (planets_positions[i].y - my_camera.transform.world_position.y) * zoom),
                    50,
                    RED);
            }
        }

        char text[128];
        sprintf(text, "Target zoom: %f", my_camera.target_zoom);
        DrawText(text, 0, 10, 20, WHITE);
        sprintf(text, "Actual zoom: %f", my_camera.zoom);
        DrawText(text, 0, 30, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
