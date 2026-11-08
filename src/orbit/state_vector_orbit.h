#pragma once
#include "../math_types.h"
#include "orbit.h"

orbit svo_generate_orbit(double gravitational_parameter, vec2d position, vec2d velocity);

double svo_argument_of_periapsis(vec2d eccentric_vector);

vec2d svo_eccentric_vector(double gravitational_parameter, vec2d position, vec2d velocity);

double svo_orbital_angular_momentum(vec2d position, vec2d velocity);

double svo_orbital_period(double gravitational_parameter, double semi_major_axis);

double svo_semi_major_axis(double relative_pos_magnitude, double v2, double gravitational_parameter);
