#!/bin/bash

set -e

g++ -std=c++17 -Wall -Wextra -pedantic tests/boids_tests.cpp -o boids_tests
./boids_tests

g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o app
cli_output=$(mktemp)
trap 'rm -f "$cli_output"' EXIT

./app --seconds 0.01 --red 1 --green 0 --blue 0 > "$cli_output"
grep -Fq $'\033[31m' "$cli_output"
if grep -Fq $'\033[32m' "$cli_output" || grep -Fq $'\033[34m' "$cli_output"; then
    exit 1
fi

./app --seconds 0.01 > "$cli_output"
grep -Fq $'\033[31m' "$cli_output"
grep -Fq $'\033[32m' "$cli_output"
grep -Fq $'\033[34m' "$cli_output"

./app --seconds 0.01 --red 0 > "$cli_output"
if grep -Fq $'\033[31m' "$cli_output"; then
    exit 1
fi
grep -Fq $'\033[32m' "$cli_output"
grep -Fq $'\033[34m' "$cli_output"

./app --seconds 0.01 --red 0 --green 0 --blue 0 > "$cli_output"
if grep -Fq $'\033[31m' "$cli_output" || grep -Fq $'\033[32m' "$cli_output" || grep -Fq $'\033[34m' "$cli_output"; then
    exit 1
fi

if ./app 0.01 > /dev/null 2>&1; then
    exit 1
fi
if ./app --red 400 --green 1 --blue 0 > /dev/null 2>&1; then
    exit 1
fi
if ./app --red 1 --red 2 > /dev/null 2>&1; then
    exit 1
fi
if ./app --orange 1 > /dev/null 2>&1; then
    exit 1
fi
if ./app --green > /dev/null 2>&1; then
    exit 1
fi
if ./app --blue -1 > /dev/null 2>&1; then
    exit 1
fi
