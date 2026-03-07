#!/usr/bin/env bash
set -e

echo "Cleaning build directory..."
rm -rf build

echo "Configuring with Meson..."
meson setup build

echo "Compiling..."
meson compile -C build

echo "Compiling GSettings schema..."
glib-compile-schemas data

echo "Running servctl..."

export GSETTINGS_SCHEMA_DIR=data
export G_MESSAGES_DEBUG=all

exec ./build/servctl
