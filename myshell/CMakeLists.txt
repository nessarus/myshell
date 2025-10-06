# CMake configuration file for the 'myshell' project.
# This file defines the build process for a simple C-based shell application,
# including compilation standards, source collection, and necessary libraries.

# ----------------------------------------------
# 1. Minimum CMake Version and Project Setup
# ----------------------------------------------
# Requires CMake version 3.12 or newer to use the CONFIGURE_DEPENDS feature.
cmake_minimum_required(VERSION 3.12)

# Defines the project name and enables the C language support.
project(myshell C)

# ----------------------------------------------
# 2. Source Files and Executable Target Definition
# ----------------------------------------------

# Collect all C source files (.c) in the current directory and the 'src' subdirectory.
# 'CONFIGURE_DEPENDS' ensures CMake automatically re-runs if files are added/removed.
file(GLOB SOURCE_FILES CONFIGURE_DEPENDS
    "*.c"
    "src/*.c"
)

# Collect all header files (.h) in the 'include' subdirectory for IDE organization.
file(GLOB HEADER_FILES CONFIGURE_DEPENDS
    "include/*.h"
)

# Declares the executable target named 'myshell' from the collected source files AND headers.
# Headers must be listed here for them to appear in the IDE project structure.
add_executable(myshell ${SOURCE_FILES} ${HEADER_FILES})

# Mark all header files as HEADER_FILE_ONLY. This includes them in the IDE
# structure but prevents the build system from compiling them as source code.
set_source_files_properties(${HEADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

# Adds the 'include' directory to the compiler's search path for the 'myshell' target.
# This allows C files to use simple includes like #include "my_header.h".
target_include_directories(myshell PRIVATE
    include
)

# ----------------------------------------------
# 3. C Standard and Compilation Enforcement
# ----------------------------------------------

# Enforce the C99 standard for the 'myshell' target.
set_property(TARGET myshell PROPERTY C_STANDARD 99)

# Require C99 strictly; compilation will fail if the standard is not met.
set_property(TARGET myshell PROPERTY C_STANDARD_REQUIRED ON)

# Apply common and strict compilation flags (warnings and diagnostics).
# The 'PRIVATE' keyword ensures these flags are not inherited by other targets.
target_compile_options(myshell PRIVATE
    -Wall                     # Enable all standard warnings
    -pedantic                 # Enforce strict adherence to the C standard
    -Werror                   # Treat all warnings as errors
    -fcolor-diagnostics       # Enable colorful diagnostics (Clang/GCC)
    -fansi-escape-codes       # Enable ANSI color codes (Clang/GCC)
)

# ----------------------------------------------
# 4. Linking Dependencies
# ----------------------------------------------

# Links the 'myshell' executable against the math library (libm).
# 'PRIVATE' indicates that this dependency is only needed internally by 'myshell'.
target_link_libraries(myshell PRIVATE m)

# ----------------------------------------------
# 5. IDE Organization (Aesthetic)
# ----------------------------------------------

# Group source and header files into virtual folders for better IDE navigation.
source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Header Files" FILES ${HEADER_FILES})
