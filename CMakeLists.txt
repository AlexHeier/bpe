cmake_minimum_required(VERSION 3.16)
project(bpe-training)

set(CMAKE_CXX_STANDARD 17)

# Include PostgreSQL
include_directories("C:/Program Files/PostgreSQL/17/include")
link_directories("C:/Program Files/PostgreSQL/17/lib")

# Gather all source files
file(GLOB_RECURSE SOURCES
    main.cpp
    training/*.cpp
    vocab/*.cpp
)

add_executable(bpe-training ${SOURCES})

# Link PostgreSQL
target_link_libraries(bpe-training pq)
