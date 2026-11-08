#include "rocket_collision.h"

// Checks for collisions between the set of rockets in the same sphere of influence (SOI)
// Collisions are either between two rockets or between a rocket and the terrain
// Using the SAT algorithm
// returns rocket collision count
int rockets_check_collision(
    planet_generator planet_body,
    rocket_body* rockets_in_sphere,
    int rocket_count,
    collision_result* results) {

    int collision_count = 0;

    for (int i = 0; i < rocket_count; i++) {

    }

    return collision_count;
}
