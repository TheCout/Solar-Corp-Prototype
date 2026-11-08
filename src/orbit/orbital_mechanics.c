#include "orbital_mechanics.h"
#include "hke-sdg/hyperk.h"
#include <math.h>

#include "../my_units.h"

const double THRESH = 1E-12;
/* If the eccentricity is very close to parabolic,  and the eccentric
anomaly is quite low,  you can get an unfortunate situation where
roundoff error keeps you from converging.  Consider the just-barely-
elliptical case,  where in Kepler's equation,

M = E - e sin( E)

E and e sin( E) can be almost identical quantities.  To
around this,  near_parabolic( ) computes E - e sin( E) by expanding
the sine function as a power series:

E - e sin( E) = E - e( E - E^3/3! + E^5/5! - ...)
= (1-e)E + e( -E^3/3! + E^5/5! - ...)

It's a little bit expensive to do this,  and you only need do it
quite rarely.  (I only encountered the problem because I had orbits
that were supposed to be 'pure parabolic',  but due to roundoff,
they had e = 1+/- epsilon,  with epsilon _very_ small.)  So 'near_parabolic'
is only called if we've gone seven iterations without converging.
https://github.com/Bill-Gray/lunar/blob/master/astfuncs.cpp
*/

static double CUBE_ROOT(double x) {
	return exp(log(x) / 3.0);
}

double om_near_parabolic(double eccentric_anomaly, double eccentricity) {
	double anom2 = eccentricity > 1.0 ? eccentric_anomaly * eccentric_anomaly : -eccentric_anomaly * eccentric_anomaly;
	double term = eccentricity * anom2 * eccentric_anomaly / 6.0;
	double rval = (1.0 - eccentricity) * eccentric_anomaly - term;
	unsigned int n = 4;

	while (fabs(term) > 1e-15) {
		term *= anom2 / (double)(n * (n + 1));
		rval -= term;
		n += 2;
	}
	return rval;
}

double om_high_eccentricity_anomaly_approximation(double e, double mean_anomaly, double min_threshold, int max_iterations) {
	double curr, err, thresh;
	double delta_curr = 1.0;
	double offset = 0.0;
	int isNegative = 0;
	int n_iter = 0;

	if (e < 1.0) {
		if (mean_anomaly < -PI || mean_anomaly > PI) {
			double tmod = fmod(mean_anomaly, PI * 2);

			if (tmod > PI)             // bring mean anom within -pi to +pi
				tmod -= 2.0 * PI;
			else if (tmod < -PI)
				tmod += 2.0 * PI;
			offset = mean_anomaly - tmod;
			mean_anomaly = tmod;
		}
	}

	if (mean_anomaly < 0.0) {
		mean_anomaly = -mean_anomaly;
		isNegative = 1;
	}

	curr = mean_anomaly;
	thresh = THRESH * fabs(1.0 - e);

	if (thresh < min_threshold)
		thresh = min_threshold;

	if (e > 1.0 && mean_anomaly / e > 3.0)                       // Hyperbolic, large-mean-anomaly
		curr = log(mean_anomaly / e) + 0.85;
	if ((e > 0.8 && mean_anomaly < PI / 3.0) || e > 1.0) {  // Up to 60 degrees
		double trial = mean_anomaly / fabs(1.0 - e);

		if (trial * trial > 6.0 * fabs(1.0 - e))             // Cubic term is dominant
			trial = cbrt(6.0 * mean_anomaly);
		curr = trial;
	}

	if (e < 1.0) {
		while (fabs(delta_curr) > thresh) {
			n_iter++;
			if (n_iter > max_iterations)
				err = om_near_parabolic(curr, e) - mean_anomaly;
			else
				err = curr - e * sin(curr) - mean_anomaly;
			delta_curr = -err / (1.0 - e * cos(curr));
			curr += delta_curr;

			if (n_iter > 20) {
				break;
			}
		}
	}
	else {
		while (fabs(delta_curr) > thresh) {
			if (n_iter++ > max_iterations && e < 1.01)
				err = -om_near_parabolic(curr, e) - mean_anomaly;
			else
				err = e * (curr)-curr - mean_anomaly;
			delta_curr = -err / (e * cosh(curr) - 1.0);
			curr += delta_curr;

			if (n_iter > 20) {
				break;
			}
		}
	}
	return isNegative ? offset - curr : offset + curr;
}

double om_low_eccentricity_anomaly_approximation_ellipse(double e, double mean_anomaly, double tolerance, int max_iterations) {
	double initial_guess = mean_anomaly;
	double eccentric_anomaly = initial_guess;
	int iterations = 0;

	while (1) {
		if (++iterations >= max_iterations) {
			break;
		}

		double fx = eccentric_anomaly - e * sin(eccentric_anomaly) - mean_anomaly; // f(x)
		double fpx = 1 - e * cos(eccentric_anomaly); // f'(x)
		double deltaE = fx / fpx;
		eccentric_anomaly -= deltaE;

		if (fabs(deltaE) < tolerance) {
			break;
		}
	}
	return eccentric_anomaly;
}

double om_loop_time_since_periapsis(double period, double time_since_periapsis) {
	double full_orbit_time = period / EARTH_YEAR_PER_SECOND;
	if (time_since_periapsis >= full_orbit_time) {
		time_since_periapsis -= full_orbit_time;
	}
	return time_since_periapsis;
}

double om_mean_anomaly(int clockwise, double orbital_period, double time_since_periapsis) {
	double mean_anomaly_rad;
	mean_anomaly_rad = time_since_periapsis * PI * 2 * EARTH_YEAR_PER_SECOND / orbital_period;
	mean_anomaly_rad = fmod(fabs(mean_anomaly_rad), PI * 2);
	if (clockwise) {
		mean_anomaly_rad = 2 * PI - mean_anomaly_rad;
	}
	return mean_anomaly_rad;
}

double om_mean_anomaly_hyperbola(double u, double semi_major_axis, double time_elapsed_since_periapsis) {
	return time_elapsed_since_periapsis * sqrt(u / pow(fabs(semi_major_axis), 3));
}

double om_approximate_eccentric_anomaly_from_mean_anomaly(double eccentricity, double mean_anomaly_rad, double tolerance, int max_iterations) {
	if (eccentricity > 1) {
		double ph1;
		double residual;
		int iterations = hkepler_eq(mean_anomaly_rad, eccentricity, &ph1, &residual, tolerance);
		return ph1;
	}
	if (eccentricity < 0.9) {
		return om_low_eccentricity_anomaly_approximation_ellipse(eccentricity, mean_anomaly_rad, tolerance, max_iterations);
	}
	return om_high_eccentricity_anomaly_approximation(eccentricity, mean_anomaly_rad, tolerance, max_iterations);
}

double om_calculate_eccentric_anomaly_from_true_anomaly(double eccentricity, double true_anomaly) {
	if (eccentricity > 1) {
		return 2 * atanh(sqrt((eccentricity - 1) / (eccentricity + 1)) * tan(true_anomaly / 2));
	}

	double B = sqrt(1 - pow(eccentricity, 2));
	double eccentric_anomaly = atan2(cos(true_anomaly) * B, cos(true_anomaly) + eccentricity);
	if (eccentric_anomaly < 0) {
		eccentric_anomaly += PI * 2;
	}
	return eccentric_anomaly;
}

double om_true_anomaly(double eccentricity, double eccentric_anomaly) {
	if (eccentricity > 1) {
		return 2 * atan(sqrt((eccentricity + 1) / (eccentricity - 1)) * tanh(eccentric_anomaly / 2));
	}
	double B = eccentricity / (1 + sqrt(1 - pow(eccentricity, 2)));
	return eccentric_anomaly + 2 * atan(B * sin(eccentric_anomaly) / (1 - B * cos(eccentric_anomaly)));
}


// HELPER FUNCTIONS

orbit_elements om_orbit_update_elapsed_time(orbit my_orbit, orbit_elements elements) {
    if (my_orbit.eccentricity < 1) {
        elements.mean_anomaly = om_mean_anomaly(my_orbit.clockwise,  my_orbit.period,  elements.time_since_periapsis);
    } else {
        elements.mean_anomaly = om_mean_anomaly_hyperbola(
            my_orbit.gravitational_parameter,
            my_orbit.semi_major_axis, elements.time_since_periapsis
        );
    }
    elements.eccentric_anomaly = om_approximate_eccentric_anomaly_from_mean_anomaly(my_orbit.eccentricity, elements.mean_anomaly, THRESH, 100);
    elements.true_anomaly = om_true_anomaly(my_orbit.eccentricity, elements.eccentric_anomaly);
    return elements;
}

double om_orbit_time_since_periapsis(orbit my_orbit, vec2d position) {
    if (my_orbit.eccentricity > 1) {
        double argPeriapRad = my_orbit.argument_of_periapsis;
        vec2d periapsis = (vec2d){ cos(argPeriapRad), sin(argPeriapRad) };
        double true_anomaly = vec2d_signed_angle(vec2d_normalized(periapsis), vec2d_normalized(position)); // VALUE ALREADY IN RADIANS
        double eccentric_anomaly = om_calculate_eccentric_anomaly_from_true_anomaly(my_orbit.eccentricity, true_anomaly);
        double mean_anomaly = my_orbit.eccentricity * sinh(eccentric_anomaly) - eccentric_anomaly;
        double time = mean_anomaly / sqrt(my_orbit.gravitational_parameter / pow(fabs(my_orbit.semi_major_axis), 3));
        return time;
    }

    double T = my_orbit.period / EARTH_YEAR_PER_SECOND;
    double arg_periapsis = my_orbit.argument_of_periapsis;
    vec2d periapsis = (vec2d){ cos(arg_periapsis), sin(arg_periapsis) };
    double true_anomaly = vec2d_signed_angle(vec2d_normalized(periapsis), vec2d_normalized(position)); // VALUE ALREADY IS IN RADIANS
    double eccentric_anomaly = om_calculate_eccentric_anomaly_from_true_anomaly(my_orbit.eccentricity, true_anomaly);
    double mean_anomaly = eccentric_anomaly - my_orbit.eccentricity * sin(eccentric_anomaly);

    if (my_orbit.clockwise) {
        mean_anomaly += 2 * (PI - mean_anomaly);
    }
    double time = mean_anomaly / (2 * PI) * T;
    return time;
}

vec2d om_orbit_get_position(orbit my_orbit, orbit_elements elements) {
    if (my_orbit.eccentricity > 1) {
        double angle = elements.true_anomaly + my_orbit.argument_of_periapsis;
        vec2d direction = (vec2d){ cos(angle), sin(angle) };
        double distance = -my_orbit.semi_major_axis * (pow(my_orbit.eccentricity, 2) - 1) /
                            (1 + my_orbit.eccentricity * cos(elements.true_anomaly));
        vec2d new_pos = vec2d_scalar(direction, distance);
        return new_pos;
    }

    double angle = elements.true_anomaly;
    if (!isnan(my_orbit.argument_of_periapsis)) {
        angle = elements.true_anomaly + my_orbit.argument_of_periapsis;
    }
    vec2d direction = (vec2d){ cos(angle), sin(angle) };
    double distance = my_orbit.semi_major_axis * (1 - my_orbit.eccentricity * cos(elements.eccentric_anomaly));
    if (distance >= 0) {
        vec2d new_pos = vec2d_scalar(direction, distance);
        return new_pos;
    }
    return VEC2D_ZERO;
}

vec2d om_orbit_get_velocity(orbit my_orbit, orbit_elements elements, double distance_to_focus) {
    if (my_orbit.eccentricity > 1) {
        double common_denominator = pow(1 + my_orbit.eccentricity * cos(elements.true_anomaly), 2);
        double drx = -sin(elements.true_anomaly) / common_denominator;
        double dry = (my_orbit.eccentricity + cos(elements.true_anomaly)) / common_denominator;
        double speed = sqrt(my_orbit.gravitational_parameter * (2 / distance_to_focus - 1 / my_orbit.semi_major_axis));
        vec2d direction = vec2d_scalar((vec2d){ drx, dry }, my_orbit.clockwise ? -1 : 1);
        double cs = cos(my_orbit.argument_of_periapsis);
        double sn = sin(my_orbit.argument_of_periapsis);
        double newx = direction.x * cs - direction.y * sn;
        double newy = direction.x * sn + direction.y * cs;
        direction.x = newx;
        direction.y = newy;
        return vec2d_scalar(vec2d_normalized(direction), speed);
    }

    vec2d direction = (vec2d) { sin(elements.true_anomaly), my_orbit.eccentricity + cos(elements.true_anomaly) };
    direction = vec2d_scalar(direction, (my_orbit.clockwise ? -1 : 1));
    double cs = cos(my_orbit.argument_of_periapsis);
    double sn = sin(my_orbit.argument_of_periapsis);
    double newx = direction.x * cs - direction.y * sn;
    double newy = direction.x * sn + direction.y * cs;
    direction.x = newx;
    direction.y = newy;
    return vec2d_scalar(vec2d_normalized(direction), sqrt(my_orbit.gravitational_parameter * (2 / distance_to_focus - 1 / my_orbit.semi_major_axis)));
}
