#include "../boids.hpp"

#include <cassert>
#include <iostream>
#include <vector>

int main() {
    using namespace boids;

    assert(wrap_coordinate(-0.25, 10.0) == 9.75);
    assert(wrap_coordinate(10.25, 10.0) == 0.25);

    const std::vector<Boid> flock{{{2.0, 1.0}, {1.0, 0.0}},
                                  {{4.0, 2.0}, {0.0, -1.0}}};
    const std::string frame = render_frame(flock, 8, 4);
    assert(frame.size() == 4 * (8 + 1));
    assert(frame[8] == '\n');
    assert(frame[17] == '\n');
    assert(frame[26] == '\n');
    assert(frame[35] == '\n');

    std::vector<Boid> lone_boid{{{3.0, 3.0}, {0.5, 0.0}}};
    update(lone_boid, 10, 10);
    assert(lone_boid[0].position.x > 3.0);
    assert(lone_boid[0].position.y == 3.0);

    std::cout << "boids tests passed\n";
}
