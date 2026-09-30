#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>
#include <vector>

namespace boids {

constexpr double max_speed = 1.0;
constexpr double max_steering = 0.05;
constexpr double neighbor_radius = 6.0;
constexpr double separation_radius = 2.0;

struct Vec2 {
    double x;
    double y;
};

enum class Color {
    red,
    green,
    blue,
};

constexpr double speed_for(Color color) {
    switch (color) {
        case Color::red:
            return max_speed;
        case Color::green:
            return max_speed * 1.5;
        case Color::blue:
            return max_speed / 1.5;
    }
    return max_speed;
}

struct Boid {
    Vec2 position;
    Vec2 velocity;
    Color color = Color::red;
};

inline bool is_finite(Vec2 vector) {
    return std::isfinite(vector.x) && std::isfinite(vector.y);
}

inline Vec2 operator+(Vec2 left, Vec2 right) {
    return {left.x + right.x, left.y + right.y};
}

inline Vec2 operator-(Vec2 left, Vec2 right) {
    return {left.x - right.x, left.y - right.y};
}

inline Vec2 operator*(Vec2 vector, double scale) {
    return {vector.x * scale, vector.y * scale};
}

inline double length(Vec2 vector) {
    return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

inline Vec2 limit(Vec2 vector, double maximum) {
    const double magnitude = length(vector);
    if (magnitude > maximum && magnitude > 0.0) {
        return vector * (maximum / magnitude);
    }
    return vector;
}

inline Vec2 direction_to(Vec2 from, Vec2 to) {
    return to - from;
}

inline double clamp_coordinate(double coordinate, double limit_value) {
    const double maximum = std::nextafter(limit_value, 0.0);
    return std::clamp(coordinate, 0.0, maximum);
}

inline void bounce_coordinate(double& coordinate, double& velocity, double limit_value) {
    while (coordinate < 0.0 || coordinate > limit_value) {
        if (coordinate < 0.0) {
            coordinate = -coordinate;
        } else {
            coordinate = 2.0 * limit_value - coordinate;
        }
        velocity = -velocity;
    }
    if (coordinate == 0.0 && velocity < 0.0) {
        velocity = -velocity;
    } else if (coordinate == limit_value) {
        velocity = -velocity;
    }
    coordinate = clamp_coordinate(coordinate, limit_value);
}

inline Vec2 steer_toward(Vec2 velocity, Vec2 desired, double target_speed) {
    const double desired_length = length(desired);
    if (desired_length == 0.0) {
        return {};
    }
    desired = desired * (target_speed / desired_length);
    return limit(desired - velocity, max_steering);
}

inline void update(std::vector<Boid>& flock, int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }

    std::vector<Vec2> accelerations(flock.size());

    for (std::size_t index = 0; index < flock.size(); ++index) {
        const Boid& boid = flock[index];
        if (!is_finite(boid.position) || !is_finite(boid.velocity)) {
            continue;
        }
        Vec2 separation{};
        Vec2 alignment{};
        Vec2 cohesion{};
        const double boid_speed = speed_for(boid.color);
        int neighbors = 0;

        for (std::size_t other_index = 0; other_index < flock.size(); ++other_index) {
            if (index == other_index) {
                continue;
            }
            if (!is_finite(flock[other_index].position) || !is_finite(flock[other_index].velocity)) {
                continue;
            }

            const Vec2 offset = direction_to(boid.position, flock[other_index].position);
            const double distance = length(offset);
            if (distance > neighbor_radius) {
                continue;
            }

            if (distance == 0.0) {
                separation = separation + Vec2{index < other_index ? -1.0 : 1.0, 0.0};
                ++neighbors;
                continue;
            }

            alignment = alignment + flock[other_index].velocity;
            cohesion = cohesion + offset;
            ++neighbors;
            if (distance < separation_radius) {
                separation = separation - offset * (1.0 / (distance * distance));
            }
        }

        if (neighbors > 0) {
            const double neighbor_count = static_cast<double>(neighbors);
            alignment = alignment * (1.0 / neighbor_count);
            cohesion = cohesion * (1.0 / neighbor_count);
            const Vec2 steering = steer_toward(boid.velocity, separation, boid_speed) * 1.5 +
                                  steer_toward(boid.velocity, alignment, boid_speed) +
                                  steer_toward(boid.velocity, cohesion, boid_speed);
            accelerations[index] = limit(steering, max_steering);
        }
    }

    for (std::size_t index = 0; index < flock.size(); ++index) {
        Boid& boid = flock[index];
        if (!is_finite(boid.position) || !is_finite(boid.velocity)) {
            continue;
        }
        boid.velocity = limit(boid.velocity + accelerations[index], speed_for(boid.color));
        boid.position = boid.position + boid.velocity;
        bounce_coordinate(boid.position.x, boid.velocity.x, width);
        bounce_coordinate(boid.position.y, boid.velocity.y, height);
    }
}

inline char direction_glyph(Vec2 velocity) {
    if (std::abs(velocity.x) > std::abs(velocity.y)) {
        return velocity.x >= 0.0 ? '>' : '<';
    }
    return velocity.y >= 0.0 ? 'v' : '^';
}

inline std::string render_frame(const std::vector<Boid>& flock, int width, int height) {
    if (width <= 0 || height <= 0) {
        return {};
    }

    const int row_width = width + 2;
    std::string frame;
    frame.reserve(static_cast<std::size_t>(height + 2) * (row_width + 1));

    frame.push_back('+');
    frame.append(static_cast<std::size_t>(width), '-');
    frame.append("+\n");
    for (int row = 0; row < height; ++row) {
        frame.push_back('|');
        frame.append(static_cast<std::size_t>(width), ' ');
        frame.append("|\n");
    }
    frame.push_back('+');
    frame.append(static_cast<std::size_t>(width), '-');
    frame.append("+\n");

    for (const Boid& boid : flock) {
        if (!is_finite(boid.position) || !is_finite(boid.velocity)) {
            continue;
        }
        const int column = static_cast<int>(std::floor(clamp_coordinate(boid.position.x, width)));
        const int row = static_cast<int>(std::floor(clamp_coordinate(boid.position.y, height)));
        frame[static_cast<std::size_t>(row + 1) * (row_width + 1) + column + 1] =
            direction_glyph(boid.velocity);
    }
    return frame;
}

inline const char* color_escape(Color color) {
    switch (color) {
        case Color::red:
            return "\x1b[31m";
        case Color::green:
            return "\x1b[32m";
        case Color::blue:
            return "\x1b[34m";
    }
    return "";
}

inline std::string render_colored_frame(const std::vector<Boid>& flock, int width, int height) {
    std::string frame = render_frame(flock, width, height);
    if (frame.empty()) {
        return frame;
    }

    std::vector<int> cell_colors(static_cast<std::size_t>(width) * height, -1);
    for (const Boid& boid : flock) {
        if (!is_finite(boid.position) || !is_finite(boid.velocity)) {
            continue;
        }
        const int column = static_cast<int>(std::floor(clamp_coordinate(boid.position.x, width)));
        const int row = static_cast<int>(std::floor(clamp_coordinate(boid.position.y, height)));
        cell_colors[static_cast<std::size_t>(row) * width + column] = static_cast<int>(boid.color);
    }

    const int row_width = width + 2;
    std::string colored_frame;
    colored_frame.reserve(frame.size() + flock.size() * 9);
    for (std::size_t index = 0; index < frame.size(); ++index) {
        const int row = static_cast<int>(index / (row_width + 1)) - 1;
        const int column = static_cast<int>(index % (row_width + 1)) - 1;
        if (row >= 0 && row < height && column >= 0 && column < width) {
            const int color = cell_colors[static_cast<std::size_t>(row) * width + column];
            if (color >= 0) {
                colored_frame.append(color_escape(static_cast<Color>(color)));
                colored_frame.push_back(frame[index]);
                colored_frame.append("\x1b[0m");
                continue;
            }
        }
        colored_frame.push_back(frame[index]);
    }
    return colored_frame;
}

}  // namespace boids
