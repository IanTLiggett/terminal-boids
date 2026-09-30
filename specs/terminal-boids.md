# Spec: Minimal Terminal Boids Demo

## Objective

Provide a dependency-free C++ program that shows a small boids flock in an ANSI-capable terminal. The program is for a student learning the core flocking algorithm. Success means a fixed-size text grid with an ASCII boundary redraws without changing shape, boids visibly move as a flock, and the program exits cleanly after the requested run duration.

## Tech Stack

- C++17 and the standard library only.
- ANSI escape sequences for terminal redraw; no graphics, UI, or boids libraries.

## Commands

Build demo: `g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o app`

Run demo for the default six seconds with 8 boids of each color: `./app`

Run demo with named options: `./app --seconds 2.5 --red 12 --green 8 --blue 4`

Build and run deterministic checks: `g++ -std=c++17 -Wall -Wextra -pedantic tests/boids_tests.cpp -o boids_tests && ./boids_tests`

Run the project test script: `./test_runner.sh`

## Project Structure

- `boids.hpp` contains the small simulation model and frame renderer shared by the demo and tests.
- `main.cpp` owns terminal setup, animation timing, and program entry.
- `tests/boids_tests.cpp` contains deterministic unit checks.
- `specs/terminal-boids.md` is this feature contract.

The renderer treats its configured width and height as the drawable interior, then encloses it with a one-cell ASCII `+---+` / `|...|` boundary. This preserves compatibility with all ANSI-capable terminals; Unicode rendering is not assumed. The terminal-output renderer assigns each boid a persistent red, green, or blue ANSI color while keeping the plain framebuffer free of escape sequences for deterministic checks. Color groups also use separate maximum/target speeds: red `1.0`, green `1.5`, and blue `2/3`; steering remains shared and bounded.

## Code Style

Use simple value types, clear lowercase helper functions, fixed `constexpr` defaults, and no framework abstractions.

```cpp
for (Boid& boid : boids) {
    boid.position += boid.velocity;
    bounce_coordinate(boid.position.x, boid.velocity.x, width);
    bounce_coordinate(boid.position.y, boid.velocity.y, height);
}
```

## Testing Strategy

Tests will use fixed boid positions and velocities rather than random animation. They will verify wall reflection, a constant-width framebuffer with newlines, and safe updates when a boid has no neighbors. The test script builds and runs those checks before the demo is considered complete. The demo accepts named options for duration and color-group counts, and rejects invalid argument lists with usage text.

The command format is `./app [--seconds <positive-seconds>] [--red <0-400>] [--green <0-400>] [--blue <0-400>]`. Options may appear in any order at most once. Omitted options use six seconds and eight boids per color. Counts are non-negative decimal integers and their total cannot exceed 400.

## Plan Maintenance

For the foreseeable future, every addition to this project must be documented in this plan before or alongside its implementation. Record the addition's scope, affected files, user-visible behavior, and verification steps; update the relevant success criteria and boundaries when they change.

## Boundaries

- Always: build with warnings enabled, run deterministic tests, render each frame from a fresh fixed-size buffer, restore the terminal cursor at normal completion.
- Ask first: add dependencies, change the project toolchain, add interactive input, or change the default six-second duration.
- Never: add obstacles, resizing support, persistence, or non-core flocking features; use multi-column boid glyphs or ANSI-decorated glyphs inside the plain frame buffer.

## Success Criteria

- The demo uses separation, alignment, and cohesion with bounded speed and steering.
- Boids reflect off all four world edges, preserving overshoot and reversing only the velocity component that hit a wall; opposite edges are not flocking neighbors.
- Every rendered interior row contains exactly the configured number of visible cells and is enclosed by an ASCII boundary; boid glyphs are one-column ASCII direction characters.
- Each boid is assigned one persistent color from the red, green, and blue ANSI palette; only the terminal-output renderer adds those escape sequences.
- Red, green, and blue boids use maximum/target speeds of `1.0`, `1.5`, and `2/3` respectively; their steering limit is unchanged.
- The demo clears once, redraws from the terminal origin, hides the cursor during animation, and restores it after the requested duration.
- Command-line counts create exactly the requested red, green, and blue boid groups, with zero allowed for any group and 400 as the total limit.
- Tests pass without external dependencies.

## Open Questions

None. Defaults are a small fixed grid, 8 boids of each color, 30 FPS, and a six-second run; callers may supply named duration and color-group options.
