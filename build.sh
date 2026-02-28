#!/usr/bin/env bash
set -e

echo "Cleaning build directory..."
rm -rf build

echo "Configuring with Meson..."
meson setup build

echo "Compiling..."
meson compile -C build

echo "Running servctl..."
exec ./build/servctl
