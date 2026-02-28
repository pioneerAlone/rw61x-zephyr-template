# CMakeLists.txt — Zephyr application build entry point
# This file is the root of the Zephyr build system for this application.
# It locates the Zephyr SDK, defines the project, and specifies source files.

cmake_minimum_required(VERSION 3.20.0)

# Find Zephyr SDK — ZEPHYR_BASE must be set by west or manually
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Project name (used in build artifacts)
project(rw61x_template)

# Add application source files
target_sources(app PRIVATE src/main.c)
