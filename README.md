# Terminal Boids

A small C++17 ANSI terminal animation.

## Run

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o app
./app
```

Customize the duration and color-group sizes:

```bash
./app --seconds 2.5 --red 12 --green 8 --blue 4
```

## Test

```bash
./test_runner.sh
```

## Container

This repository is compatible with [cpp-container](https://github.com/ChicoState/cpp-container). If not already built on your machine, clone and build it.

```bash
docker run -v "$(pwd)":/usr/src -it cpp-container
```

Run an interactive shell in the container:

```bash
docker run -v "$(pwd)":/usr/src -it cpp-container sh
```
