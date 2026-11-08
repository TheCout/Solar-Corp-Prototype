#include <math.h>
#include "camera.h"
#include "../my_units.h"

struct camera camera_default() {
    struct camera cam = (struct camera) {
        .ZOOM_SPEED = 0.1,
        .ZOOM_ADJUST_SPEED = 12.0,
        .ZOOM_BASE = 2.8,
        .zoom_exponent = -10,
    };
    cam.target_zoom = powf(cam.ZOOM_BASE, cam.zoom_exponent);
    return cam;
}

void camera_set_target_zoom(struct camera* my_cam, double target_zoom) {
    my_cam->target_zoom = target_zoom;
    my_cam->zoom_exponent = log(target_zoom) / log(my_cam->ZOOM_BASE);
}

void camera_zoom_input(struct camera* my_cam, float y) {
    my_cam->zoom_exponent += y * fabsf(y) * my_cam->ZOOM_SPEED;
    my_cam->target_zoom = powf(my_cam->ZOOM_BASE, my_cam->zoom_exponent);
}

void camera_update_zoom(struct camera* my_cam, float dt) {
    float easing_factor = 1.0f - exp(-my_cam->ZOOM_ADJUST_SPEED * dt);
    my_cam->zoom += (my_cam->target_zoom - my_cam->zoom) * easing_factor;
}

void camera_update_dimensions(struct camera* my_cam, int screen_width, int screen_height) {
    my_cam->width = screen_width / my_cam->zoom;
    my_cam->height = screen_height / my_cam->zoom;
}

void camera_angular_aperture_from_point(
    struct camera my_cam,
    vec2d pos,
    double* angle0out,
    double* angle1out) {

    double hw = my_cam.width;
    double hh = my_cam.height;

    double left_x = -pos.x - hw;
    double top_y = -pos.y - hh;
    double right_x = -pos.x + hw;
    double bottom_y = -pos.y + hh;

    vec2d tl = (vec2d) { .x = left_x, .y = top_y};
    vec2d tr = (vec2d) { .x = right_x, .y = top_y};
    vec2d bl = (vec2d) { .x = left_x, .y = bottom_y};
    vec2d br = (vec2d) { .x = right_x, .y = bottom_y};

    double tr_ang = atan2(top_y, right_x);
    if (tr_ang < 0) {
        tr_ang = tr_ang + TWO_PI;
    }

    double tl_ang = vec2d_signed_angle(tr, tl);
    double bl_ang = vec2d_signed_angle(tr, bl);
    double br_ang = vec2d_signed_angle(tr, br);

    double min_ang = 0;
    double max_ang = 0;

    if (tl_ang < min_ang) {
        min_ang = tl_ang;
    } else if (tl_ang > max_ang) {
        max_ang = tl_ang;
    }

    if (bl_ang < min_ang) {
        min_ang = bl_ang;
    } else if (bl_ang > max_ang) {
        max_ang = bl_ang;
    }

    if (br_ang < min_ang) {
        min_ang = br_ang;
    } else if (br_ang > max_ang) {
        max_ang = br_ang;
    }

    min_ang = min_ang + tr_ang;
        max_ang = max_ang + tr_ang;

    if (max_ang < min_ang) {
        max_ang += TWO_PI;
    }

    while (min_ang < 0) {
        min_ang += TWO_PI;
        max_ang += TWO_PI;
    }
    while (min_ang >= TWO_PI) {
        min_ang -= TWO_PI;
        max_ang -= TWO_PI;
    }

    *angle0out = min_ang;
    *angle1out = max_ang;
}

int camera_is_point_in_view(
    struct camera my_cam,
    vec2d circle_world_pos) {

    double cl = my_cam.transform.world_position.x - my_cam.width;
    double cr = my_cam.transform.world_position.x + my_cam.width;
    double ct = my_cam.transform.world_position.y - my_cam.height;
    double cb = my_cam.transform.world_position.y + my_cam.height;

    return circle_world_pos.x >= cl && circle_world_pos.x <= cr &&
           circle_world_pos.y >= ct && circle_world_pos.y <= cb;
}

int camera_circle_to_circle_collision(
    struct camera my_cam,
    vec2d circle_world_pos,
    double circle_radius) {

    vec2d my_pos = my_cam.transform.world_position;
    float dx = my_pos.x - circle_world_pos.x;
    float dy = my_pos.y - circle_world_pos.y;
    float distance_sqr = (dx * dx) + (dy * dy);
    float camera_radius = sqrtf((my_cam.width * my_cam.width) + (my_cam.height * my_cam.height));
    float total_radius = camera_radius + circle_radius;
    float total_radius_sqr = total_radius * total_radius;
    return distance_sqr <= total_radius_sqr;
}
