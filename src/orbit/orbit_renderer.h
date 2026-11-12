#ifndef ORBIT_RENDERER
#define ORBIT_RENDERER

#include <raylib.h>
#include "../general_math/math_types.h"
#include "orbit.h"

// Creates the Shader which will be used for rendering orbits
// This function reads a vertex and fragment shader. It then compiles and configures them for later use
// vs_path -> Vertex shader path (string)
// fs_path -> Fragment shader path (string)
void orbit_load_renderer_shader(const char* vs_path, const char* fs_path);

void orbit_unload_renderer_shader();

// Generates a point list of a section of the orbit trajectory
// Angle B must be larger than Angle A
// returns number of points written
int orbit_generate_points(const orbit* my_orbit, double anglea, double angleb, int resolution, vec2d* points);

#endif
