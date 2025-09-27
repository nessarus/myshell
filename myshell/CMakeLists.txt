# CMake configuration file for the 'myshell' project.
# This file declares an executable target, sets required C standards,
# configures compiler options, and links necessary libraries.

# ----------------------------------------------
# 1. Minimum CMake Version and Project Setup
# ----------------------------------------------
# Specifies the minimum version of CMake required for the project.
cmake_minimum_required(VERSION 3.10)

# Defines the project name and enables the C language support.
project(myshell C)

# ----------------------------------------------
# 2. Source Files and Executable Target Definition
# ----------------------------------------------

# Gather all C source files in the current directory.
# NOTE: file(GLOB) is used here to dynamically collect sources.
file(GLOB SOURCE_FILES "*.c")

# Declares the executable target named 'myshell' from the collected source files.
# This command MUST be executed before any properties are set on the target.
add_executable(myshell ${SOURCE_FILES})

# ----------------------------------------------
# 3. C Standard and Compilation Enforcement
# ----------------------------------------------

# Set the C standard to C99.
set_property(TARGET myshell PROPERTY C_STANDARD 99)
# Make the C99 standard mandatory for compilation.
set_property(TARGET myshell PROPERTY C_STANDARD_REQUIRED ON)

# Apply compilation flags (warnings and diagnostics) exclusively to the 'myshell' target.
# The 'PRIVATE' keyword ensures these flags are not inherited by other targets.
target_compile_options(myshell PRIVATE
    -Wall                   # Enable all standard warnings
    -pedantic               # Enforce strict adherence to the C standard
    -Werror                 # Treat all warnings as errors
    -fcolor-diagnostics     # Enable colorful diagnostics (Clang/GCC)
    -fansi-escape-codes     # Enable ANSI color codes (Clang/GCC)
)

# ----------------------------------------------
# 4. Linking Dependencies
# ----------------------------------------------

# Links the 'myshell' executable against the math library (m).
# 'PRIVATE' indicates the dependency is internal to this target.
target_link_libraries(myshell PRIVATE m)
