#include "../boids.hpp"

#include <cassert>
#include <cmath>
#include <limits>
#include <iostream>
#include <vector>

int main() {
    using namespace boids;

    const std::vector<Boid> flock{{{2.0, 1.0}, {1.0, 0.0}},
                                  {{4.0, 2.0}, {0.0, -1.0}}};
    const std::string frame = render_frame(flock, 8, 4);
    assert(frame.size() == 6 * 11);
    assert(frame.substr(0, 11) == "+--------+\n");
    assert(frame.substr(11, 11) == "|        |\n");
    assert(frame.substr(55, 11) == "+--------+\n");
    assert(frame[2 * 11 + 1 + 2] == '>');
    assert(frame[3 * 11 + 1 + 4] == '^');
    assert(render_frame(flock, 0, 4).empty());

    const std::vector<Boid> outside_boid{{{10.2, 1.0}, {1.0, 0.0}}};
    const std::string clamped_frame = render_frame(outside_boid, 8, 4);
    assert(clamped_frame[2 * 11 + 1 + 7] == '>');

    const std::vector<Boid> colored_flock{{{2.0, 0.0}, {1.0, 0.0}, Color::red},
                                          {{4.0, 1.0}, {0.0, 1.0}, Color::green},
                                          {{6.0, 2.0}, {-1.0, 0.0}, Color::blue}};
    const std::string colored_frame = render_colored_frame(colored_flock, 8, 4);
    assert(colored_frame.substr(0, 11) == "+--------+\n");
    assert(colored_frame.find("\x1b[31m>\x1b[0m") != std::string::npos);
    assert(colored_frame.find("\x1b[32mv\x1b[0m") != std::string::npos);
    assert(colored_frame.find("\x1b[34m<\x1b[0m") != std::string::npos);

    assert(speed_for(Color::red) == 1.0);
    assert(speed_for(Color::green) == 1.5);
    assert(std::abs(speed_for(Color::blue) - (1.0 / 1.5)) < 0.000001);
    std::vector<Boid> speed_groups{{{1.0, 1.0}, {2.0, 0.0}, Color::red},
                                   {{3.0, 1.0}, {2.0, 0.0}, Color::green},
                                   {{5.0, 1.0}, {2.0, 0.0}, Color::blue}};
    update(speed_groups, 20, 20);
    assert(std::abs(length(speed_groups[0].velocity) - speed_for(Color::red)) < 0.000001);
    assert(std::abs(length(speed_groups[1].velocity) - speed_for(Color::green)) < 0.000001);
    assert(std::abs(length(speed_groups[2].velocity) - speed_for(Color::blue)) < 0.000001);

    const std::vector<Boid> invalid_boid{{{std::numeric_limits<double>::quiet_NaN(), 1.0},
                                           {1.0, 0.0}}};
    const std::string empty_frame = render_frame(invalid_boid, 8, 4);
    assert(empty_frame.size() == 6 * 11);
    assert(empty_frame[1 * 11 + 1] == ' ');

    std::vector<Boid> lone_boid{{{3.0, 3.0}, {0.5, 0.0}}};
    update(lone_boid, 10, 10);
    assert(lone_boid[0].position.x > 3.0);
    assert(lone_boid[0].position.y == 3.0);

    std::vector<Boid> horizontal_bounce{{{9.8, 5.0}, {0.5, 0.0}}};
    update(horizontal_bounce, 10, 10);
    assert(std::abs(horizontal_bounce[0].position.x - 9.7) < 0.000001);
    assert(std::abs(horizontal_bounce[0].velocity.x + 0.5) < 0.000001);

    std::vector<Boid> vertical_bounce{{{5.0, 0.1}, {0.0, -0.3}}};
    update(vertical_bounce, 10, 10);
    assert(std::abs(vertical_bounce[0].position.y - 0.2) < 0.000001);
    assert(std::abs(vertical_bounce[0].velocity.y - 0.3) < 0.000001);

    std::vector<Boid> opposite_edges{{{0.2, 5.0}, {0.0, 0.0}},
                                     {{9.8, 5.0}, {0.0, 0.0}}};
    update(opposite_edges, 10, 10);
    assert(length(opposite_edges[0].velocity) == 0.0);
    assert(length(opposite_edges[1].velocity) == 0.0);

    std::vector<Boid> crowded_flock{{{5.0, 5.0}, {1.0, 0.0}},
                                     {{5.2, 5.0}, {1.0, 0.0}},
                                     {{5.0, 5.2}, {0.0, 1.0}}};
    const std::vector<Vec2> previous_velocities{{1.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};
    update(crowded_flock, 20, 20);
    for (std::size_t index = 0; index < crowded_flock.size(); ++index) {
        const Boid& boid = crowded_flock[index];
        assert(length(boid.velocity) <= max_speed + 0.000001);
        assert(length(boid.velocity - previous_velocities[index]) <= max_steering + 0.000001);
    }

    std::vector<Boid> overlapping_boids{{{5.0, 5.0}, {0.0, 0.0}},
                                         {{5.0, 5.0}, {0.0, 0.0}}};
    update(overlapping_boids, 20, 20);
    assert(overlapping_boids[0].position.x < 5.0);
    assert(overlapping_boids[1].position.x > 5.0);

    std::cout << "boids tests passed\n";
}
