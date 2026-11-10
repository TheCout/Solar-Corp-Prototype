#include "math_types.h"
#include <math.h>

int vec2d_equal(vec2d v1, vec2d v2) {
    return (v1.x == v2.x && v1.y == v2.y);
}

vec2d vec2d_addf(Vector2 v1, vec2d v2) {
    return (vec2d){v1.x + v2.x, v1.y + v2.y};
}

vec2d vec2d_add(vec2d v1, vec2d v2) {
    return (vec2d){v1.x + v2.x, v1.y + v2.y};
}

double vec2d_dot(vec2d v1, vec2d v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

double vec2d_cross(vec2d v1, vec2d v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

double vec2d_signed_angle(vec2d v1, vec2d v2) {
    double angle = acos(vec2d_dot(v1, v2) / (vec2d_magnitude(v1) * vec2d_magnitude(v2)));
    double cross = vec2d_cross(v1, v2);
    return cross < 0 ? -angle : angle;
}

double vec2d_magnitude(vec2d v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

double vec2d_sqr_magnitude(vec2d v) {
    return v.x * v.x + v.y * v.y;
}

vec2d vec2d_normalized(vec2d v) {
    double mag = vec2d_magnitude(v);
    return (vec2d){v.x / mag, v.y / mag};
}

vec2d vec2d_scalar(vec2d v, double scalar) {
    return (vec2d){v.x * scalar, v.y * scalar};
}
