# Spec: Minimal Terminal Boids Demo

## Objective

Provide a dependency-free C++ program that shows a small boids flock in an ANSI-capable terminal. The program is for a student learning the core flocking algorithm. Success means a fixed-size text grid redraws without changing shape, boids visibly move as a flock, and the program exits cleanly after a fixed run.

## Tech Stack

- C++17 and the standard library only.
- ANSI escape sequences for terminal redraw; no graphics, UI, or boids libraries.

## Commands

Build demo: `g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o app`

Run demo: `./app`

Build and run deterministic checks: `g++ -std=c++17 -Wall -Wextra -pedantic tests/boids_tests.cpp -o boids_tests && ./boids_tests`

Run the project test script: `./test_runner.sh`

## Project Structure

- `boids.hpp` contains the small simulation model and frame renderer shared by the demo and tests.
- `main.cpp` owns terminal setup, animation timing, and program entry.
- `tests/boids_tests.cpp` contains deterministic unit checks.
- `specs/terminal-boids.md` is this feature contract.

## Code Style

Use simple value types, clear lowercase helper functions, fixed `constexpr` defaults, and no framework abstractions.

```cpp
for (Boid& boid : boids) {
    boid.position += boid.velocity;
    wrap(boid.position, width, height);
}
```

## Testing Strategy

Tests will use fixed boid positions and velocities rather than random animation. They will verify position wrapping, a constant-width framebuffer with newlines, and safe updates when a boid has no neighbors. The test script builds and runs those checks before the demo is considered complete.

## Boundaries

- Always: build with warnings enabled, run deterministic tests, render each frame from a fresh fixed-size buffer, restore the terminal cursor at normal completion.
- Ask first: add dependencies, change the project toolchain, add interactive input, change the planned fixed-duration behavior.
- Never: add colors, obstacles, resizing support, persistence, or non-core flocking features; use multi-column or ANSI-decorated boid glyphs inside the frame buffer.

## Success Criteria

- The demo uses separation, alignment, and cohesion with bounded speed and steering.
- Boids wrap through all four world edges.
- Every rendered row contains exactly the configured number of visible cells; boid glyphs are one-column ASCII direction characters.
- The default demo clears once, redraws from the terminal origin, hides the cursor during animation, and restores it after its fixed frame count.
- Tests pass without external dependencies.

## Open Questions

None. Defaults are a small fixed grid, roughly two dozen boids, 30 FPS, and a short fixed run.
