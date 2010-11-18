# CMakeList top directory
#------------------------------------------
# The name of our project is "TISERCONE". CMakeLists files in this project can
# refer to the root source directory of the project as ${TISERCONE_SOURCE_DIR} and
# to the root binary directory of the project as ${TISERCONE_BINARY_DIR}.
cmake_minimum_required (VERSION 2.6)
project (TISERCONE)

# Compiler flags
add_definitions(-Wall)

# Add include directory
#include_directories(${include)

# Recurse into the "src", "include" and "bin" subdirectories. This does not actually
# cause another cmake executable to run. The same process will walk through
# the project's entire directory structure.
add_subdirectory (src)
add_subdirectory (bin)

