cmake_minimum_required(VERSION 3.20)
project(multithreaded_merge_sort C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(src)

add_executable(p_merge_sort
        src/p_merge_sort.c
        src/multithreading.c
        src/multithreading.h
        src/verbosity.c
        src/verbosity.h)

add_executable(trad_merge_sort
        src/trad_merge_sort.c
        src/verbosity.c
        src/verbosity.h)

# Include directories
target_include_directories(p_merge_sort PUBLIC
        "${PROJECT_BINARY_DIR}"
)

# Link pthread
find_package(Threads REQUIRED)
target_link_libraries(p_merge_sort Threads::Threads m)

# Set the directory where the executables will be stored
set_target_properties(p_merge_sort trad_merge_sort PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
