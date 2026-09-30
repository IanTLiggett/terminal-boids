#include "boids.hpp"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

namespace {

constexpr int world_width = 64;
constexpr int world_height = 20;
constexpr double default_run_seconds = 6.0;
constexpr std::size_t default_group_count = 8;
constexpr std::size_t maximum_boid_count = 400;
constexpr auto frame_duration =
    std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(1.0 / 30.0));

struct DemoOptions {
    double run_seconds = default_run_seconds;
    std::size_t red_count = default_group_count;
    std::size_t green_count = default_group_count;
    std::size_t blue_count = default_group_count;
};

class CursorGuard {
public:
    CursorGuard() {
        std::cout << "\x1b[?25l" << std::flush;
    }

    ~CursorGuard() {
        std::cout << "\x1b[?25h" << std::flush;
    }
};

std::vector<boids::Boid> make_flock(const DemoOptions& options) {
    std::mt19937 random_engine(42);
    std::uniform_real_distribution<double> x_position(0.0, world_width);
    std::uniform_real_distribution<double> y_position(0.0, world_height);
    std::uniform_real_distribution<double> velocity(-0.7, 0.7);

    std::vector<boids::Boid> flock;
    flock.reserve(options.red_count + options.green_count + options.blue_count);
    const auto add_group = [&](std::size_t count, boids::Color color) {
        for (std::size_t index = 0; index < count; ++index) {
            flock.push_back({{x_position(random_engine), y_position(random_engine)},
                             {velocity(random_engine), velocity(random_engine)}, color});
        }
    };
    add_group(options.red_count, boids::Color::red);
    add_group(options.green_count, boids::Color::green);
    add_group(options.blue_count, boids::Color::blue);
    return flock;
}

bool parse_run_seconds(const char* text, double& run_seconds) {
    char* end = nullptr;
    errno = 0;
    const double parsed_seconds = std::strtod(text, &end);
    if (end == text || *end != '\0' || errno == ERANGE || !std::isfinite(parsed_seconds) ||
        parsed_seconds <= 0.0) {
        return false;
    }

    run_seconds = parsed_seconds;
    return true;
}

bool parse_count(const char* text, std::size_t& count) {
    if (*text == '\0') {
        return false;
    }

    std::size_t parsed_count = 0;
    for (const char* character = text; *character != '\0'; ++character) {
        if (*character < '0' || *character > '9') {
            return false;
        }
        const std::size_t digit = static_cast<std::size_t>(*character - '0');
        if (parsed_count > (maximum_boid_count - digit) / 10) {
            return false;
        }
        parsed_count = parsed_count * 10 + digit;
    }

    count = parsed_count;
    return true;
}

bool parse_options(int argument_count, char* arguments[], DemoOptions& options) {
    bool seconds_seen = false;
    bool red_seen = false;
    bool green_seen = false;
    bool blue_seen = false;

    for (int index = 1; index < argument_count; index += 2) {
        if (index + 1 >= argument_count) {
            return false;
        }

        const char* flag = arguments[index];
        const char* value = arguments[index + 1];
        if (std::strcmp(flag, "--seconds") == 0) {
            if (seconds_seen || !parse_run_seconds(value, options.run_seconds)) {
                return false;
            }
            seconds_seen = true;
        } else if (std::strcmp(flag, "--red") == 0) {
            if (red_seen || !parse_count(value, options.red_count)) {
                return false;
            }
            red_seen = true;
        } else if (std::strcmp(flag, "--green") == 0) {
            if (green_seen || !parse_count(value, options.green_count)) {
                return false;
            }
            green_seen = true;
        } else if (std::strcmp(flag, "--blue") == 0) {
            if (blue_seen || !parse_count(value, options.blue_count)) {
                return false;
            }
            blue_seen = true;
        } else {
            return false;
        }
    }

    return options.red_count + options.green_count + options.blue_count <= maximum_boid_count;
}

void print_usage(const char* program) {
    std::cerr << "Usage: " << program
              << " [--seconds <positive-seconds>] [--red <0-400>] [--green <0-400>] [--blue <0-400>]\n"
              << "The total number of boids cannot exceed 400.\n";
}

bool has_valid_duration(const DemoOptions& options, std::chrono::steady_clock::time_point start_time) {
    const double maximum_run_seconds =
        std::chrono::duration<double>(std::chrono::steady_clock::time_point::max() - start_time).count();
    return options.run_seconds < maximum_run_seconds;
}

std::chrono::steady_clock::time_point end_time_for(const DemoOptions& options,
                                                    std::chrono::steady_clock::time_point start_time) {
    return start_time + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                            std::chrono::duration<double>(options.run_seconds));
}

}  // namespace

int main(int argument_count, char* arguments[]) {
    DemoOptions options;
    const auto start_time = std::chrono::steady_clock::now();
    if (!parse_options(argument_count, arguments, options) || !has_valid_duration(options, start_time)) {
        print_usage(arguments[0]);
        return 1;
    }

    std::vector<boids::Boid> flock = make_flock(options);
    const auto end_time = end_time_for(options, start_time);

    std::cout << "\x1b[2J" << std::flush;
    const CursorGuard cursor;
    while (std::chrono::steady_clock::now() < end_time) {
        std::cout << "\x1b[H" << boids::render_colored_frame(flock, world_width, world_height)
                  << std::flush;
        boids::update(flock, world_width, world_height);
        const auto remaining = end_time - std::chrono::steady_clock::now();
        std::this_thread::sleep_for(std::min(frame_duration, remaining));
    }
    return 0;
}
