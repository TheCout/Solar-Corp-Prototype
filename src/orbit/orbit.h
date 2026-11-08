#ifndef ORBIT_H
#define ORBIT_H

typedef struct {
    double gravitational_parameter;
    double semi_major_axis;
    double eccentricity;
    double argument_of_periapsis;
    double period;
    int clockwise;
} orbit;

#endif
