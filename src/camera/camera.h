#ifndef CAMERA_H
#define CAMERA_H

#include "../math_types.h"

struct camera {
    float ZOOM_SPEED;
    float ZOOM_ADJUST_SPEED;
    float ZOOM_BASE;
    double zoom_exponent;
    double target_zoom;

    double width;
    double height;
    double zoom;
    vec2d *origin_position;
    struct transform transform;
};

struct camera camera_default();

void camera_set_target_zoom(struct camera* my_cam, double target_zoom);

void camera_zoom_input(struct camera* my_cam, float y);

void camera_update_zoom(struct camera* my_cam, float dt);

void camera_update_dimensions(struct camera* my_cam, int screen_width, int screen_height);

void camera_angular_aperture_from_point(
    struct camera my_cam,
    vec2d pos,
    double* angle0out,
    double* angle1out);

int camera_is_point_in_view(
    struct camera my_cam,
    vec2d circle_world_pos);

// returns 1 if the two circles are colliding, 0 otherwise
int camera_circle_to_circle_collision(
    struct camera my_cam,
    vec2d circle_world_pos,
    double circle_radius);

#endif
