#ifndef MATH_TYPES_H
#define MATH_TYPES_H

typedef struct {
    union {
        struct {
            float x;
            float y;
        };
        float raw[2];
    };
} vec2f;

typedef struct {
    union {
        struct {
            double x;
            double y;
        };
        double raw[2];
    };
} vec2d;

struct transform {
    vec2d local_position;
    vec2d world_position;
};

#include <raymath.h>
vec2d vec2d_addf(Vector2 v1, vec2d v2);
vec2d vec2d_add(vec2d v1, vec2d v2);

double vec2d_dot(vec2d v1, vec2d v2);
double vec2d_cross(vec2d v1, vec2d v2);
double vec2d_signed_angle(vec2d v1, vec2d v2);
double vec2d_magnitude(vec2d v);
double vec2d_sqr_magnitude(vec2d v);
vec2d vec2d_normalized(vec2d v);
vec2d vec2d_scalar(vec2d v, double scalar);

#define VEC2F_ZERO ((vec2f){0})
#define VEC2D_ZERO ((vec2d){0})

#define MAX(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a > _b ? _a : _b; \
})

#endif
