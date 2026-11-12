#include "orbit_renderer.h"
#include <rlgl.h>

Shader shader;
int loc_resolution;
int loc_thickness;

static void extrude_points(Vector2* points, int point_count, float thickness, Color color, bool closed_loop) {
        if (point_count < 2) return;

        float res[2] = { (float)GetScreenWidth(), (float)GetScreenHeight() };
        SetShaderValue(shader, loc_resolution, res, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, loc_thickness, &thickness, SHADER_UNIFORM_FLOAT);

        BeginShaderMode(shader);
        rlBegin(RL_QUADS);
        rlColor4ub(color.r, color.g, color.b, color.a);

        // If closedLoop is true (e.g. full ellipse), emitpoint_count+ 1 vertices to seal the seam
        size_t count = closed_loop ? point_count + 1 : point_count;

        for (size_t i = 0; i < count; i++) {
            // Indices for segment start (A) and segment end (B)
            size_t idxA = i;
            size_t idxB = (i + 1) % point_count;

            size_t idxPrevA = (idxA == 0) ? (closed_loop ? point_count - 1 : 0) : idxA - 1;
            size_t idxNextB = (idxB ==point_count - 1) ? (closed_loop ? 0 :point_count - 1) : idxB + 1;

            Vector2 pA     = points[idxA];
            Vector2 pB     = points[idxB];
            Vector2 pPrevA = points[idxPrevA];
            Vector2 pNextB = points[idxNextB];

            // Emit 4 vertices forming one quad segment (Counter-Clockwise)
            // Start Left (side = -1.0f)
            rlNormal3f(pPrevA.x, pPrevA.y, -1.0f);
            rlTexCoord2f(pB.x, pB.y);
            rlVertex3f(pA.x, pA.y, 0.0f);

            // Start Right (side = +1.0f)
            rlNormal3f(pPrevA.x, pPrevA.y, +1.0f);
            rlTexCoord2f(pB.x, pB.y);
            rlVertex3f(pA.x, pA.y, 0.0f);

            // End Right (side = +1.0f)
            rlNormal3f(pA.x, pA.y, +1.0f);
            rlTexCoord2f(pNextB.x, pNextB.y);
            rlVertex3f(pB.x, pB.y, 0.0f);

            // End Left (side = -1.0f)
            rlNormal3f(pA.x, pA.y, -1.0f);
            rlTexCoord2f(pNextB.x, pNextB.y);
            rlVertex3f(pB.x, pB.y, 0.0f);
        }

        rlEnd();
        EndShaderMode();
    }

void orbit_load_renderer_shader(const char* vs_path, const char* fs_path) {
     shader = LoadShader(vs_path, fs_path);
     loc_resolution = GetShaderLocation(shader, "resolution");
     loc_thickness  = GetShaderLocation(shader, "thickness");
}

void orbit_unload_renderer_shader() {
    UnloadShader(shader);
}

int orbit_generate_points(const orbit* my_orbit, double anglea, double angleb, int resolution, vec2d* points) {
    double angle = anglea;
    double amplitude = angleb - anglea;
    double delta_angle = amplitude / resolution;

    double semi_latus = my_orbit->semi_major_axis * (1 - my_orbit->eccentricity*my_orbit->eccentricity);
    semi_latus = fabs(semi_latus);

    for (int i = 0; i < resolution; i++) {
        angle += delta_angle;

        double radius = semi_latus / (1 + my_orbit->eccentricity * cos(angle));
        points[i] = (vec2d) {
            .x = radius * cos(angle),
            .y = radius * sin(angle)
        };
    }
    return resolution;
}

void orbit_render_points(
