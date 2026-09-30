#!/bin/bash

g++ -std=c++17 -Wall -Wextra -pedantic tests/boids_tests.cpp -o boids_tests
./boids_tests
