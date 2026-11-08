#include "state_vector_orbit.h"
#include <math.h>
#include "../my_units.h"

#define DBL_EPSILON 1E-15

orbit svo_generate_orbit(double gravitational_parameter, vec2d position, vec2d velocity) {
    double position_magnitude = vec2d_magnitude(position);
    double angular_momentum = svo_orbital_angular_momentum(position, velocity);
    double v2 = vec2d_sqr_magnitude(velocity);

    vec2d eccentric_vector = svo_eccentric_vector(gravitational_parameter, position, velocity);
    double eccentricity = vec2d_magnitude(eccentric_vector);

    orbit orbit = {
        .eccentricity = eccentricity,
        .argument_of_periapsis = svo_argument_of_periapsis(eccentric_vector)
    };

    if (fabs(eccentricity - 1.0) > DBL_EPSILON) {
        if (eccentricity > 1) {
            orbit.semi_major_axis = svo_semi_major_axis(position_magnitude, v2, gravitational_parameter);
        }
        else {
            orbit.semi_major_axis = svo_semi_major_axis(position_magnitude, v2, gravitational_parameter);
            orbit.period = svo_orbital_period(gravitational_parameter, orbit.semi_major_axis);
        }
    }
    else {
        // not implemented yet!
    }

    orbit.clockwise = angular_momentum < 0;
    orbit.gravitational_parameter = gravitational_parameter;
    return orbit;
}

double svo_argument_of_periapsis(vec2d eccentric_vector) {
    double argument_of_periapsis = atan2(eccentric_vector.y, eccentric_vector.x);
    if (eccentric_vector.y < 0) {
        argument_of_periapsis += PI * 2;
    }
    return argument_of_periapsis;
}

vec2d svo_eccentric_vector(double gravitational_parameter, vec2d position, vec2d velocity) {
    double z = (velocity.x * -position.y) - (velocity.y * -position.x);

    double x = ( velocity.y * z) / gravitational_parameter;
    double y = (-velocity.x * z) / gravitational_parameter;
    vec2d normalized_pos = vec2d_normalized(position);
    vec2d result = { 0 };
    result.x = x - normalized_pos.x;
    result.y = y - normalized_pos.y;
    return result;
}

double svo_orbital_angular_momentum(vec2d position, vec2d velocity) {
    return position.x * velocity.y - position.y * velocity.x;
}

double svo_orbital_period(double gravitational_parameter, double semi_major_axis) {
    double meanMotion = sqrt(gravitational_parameter / pow(semi_major_axis, 3));
    // orbital Period here is expressed in seconds, convert it to year
    return 2 * PI / meanMotion * EARTH_YEAR_PER_SECOND;
}

double svo_semi_major_axis(double relative_pos_magnitude, double v2, double gravitational_parameter) {
    double orbital_energy = (v2 / 2) - (gravitational_parameter / relative_pos_magnitude);
    return -(gravitational_parameter / (2 * orbital_energy));
}
