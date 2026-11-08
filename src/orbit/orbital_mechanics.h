#pragma once
#include "orbit.h"
#include "../math_types.h"

// HELPER STRUCTURES

typedef struct {
    double time_since_periapsis;
    double mean_anomaly;
    double eccentric_anomaly;
    double true_anomaly;
} orbit_elements;

// FUNCTIONS

double om_near_parabolic(double eccentric_anomaly, double eccentricity);

double om_high_eccentricity_anomaly_approximation(double e, double mean_anomaly, double min_threshold, int max_iterations);

double om_low_eccentricity_anomaly_approximation_ellipse(double e, double mean_anomaly, double tolerance, int max_iterations);

double om_loop_time_since_periapsis(double period, double time_since_periapsis);

double om_mean_anomaly(int clockwise, double orbital_period, double time_since_periapsis);

double om_mean_anomaly_hyperbola(double u, double semi_major_axis, double time_elapsed_since_periapsis) ;

double om_approximate_eccentric_anomaly_from_mean_anomaly(double eccentricity, double mean_anomaly_rad, double tolerance, int max_iterations);

double om_calculate_eccentric_anomaly_from_true_anomaly(double eccentricity, double true_anomaly);

double om_true_anomaly(double eccentricity, double eccentric_anomaly);


// HELPER FUNCTIONS

orbit_elements om_orbit_update_elapsed_time(orbit my_orbit, orbit_elements elements);
double om_orbit_time_since_periapsis(orbit my_orbit, vec2d position);
vec2d om_orbit_get_position(orbit my_orbit, orbit_elements elements);
vec2d om_orbit_get_velocity(orbit my_orbit, orbit_elements elements, double distance_to_focus);
