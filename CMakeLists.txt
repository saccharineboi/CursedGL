cmake_minimum_required(VERSION 3.10)
project(cursedgl)

# C settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
SET(CMAKE_AR  "gcc-ar")
SET(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
SET(CMAKE_C_ARCHIVE_FINISH   true)

list(APPEND CMAKE_C_FLAGS "-Wall \
                           -Wextra \
                           -Werror \
                           -Wpedantic \
                           -O1 \
                           -g \
                           -flto \
                           -Wfloat-equal \
                           -Wundef \
                           -Wshadow \
                           -Wpointer-arith \
                           -Wcast-align \
                           -Wswitch-enum \
                           -Wcast-qual \
                           -Wconversion \
                           -Wduplicated-cond \
                           -Wduplicated-branches \
                           -march=native \
                           -Winit-self \
                           -Wmissing-include-dirs")

# add source files
set(SOURCE_FILES    ${CMAKE_SOURCE_DIR}/src/framebuffer.c
                    ${CMAKE_SOURCE_DIR}/src/init.c
                    ${CMAKE_SOURCE_DIR}/src/lights.c
                    ${CMAKE_SOURCE_DIR}/src/material.c
                    ${CMAKE_SOURCE_DIR}/src/objLoader.c
                    ${CMAKE_SOURCE_DIR}/src/rasterizer.c
                    ${CMAKE_SOURCE_DIR}/src/shapes.c
                    ${CMAKE_SOURCE_DIR}/src/transform.c
                    ${CMAKE_SOURCE_DIR}/src/error.c
                    ${CMAKE_SOURCE_DIR}/src/texture.c)

# add header files
set(HEADER_FILES    ${CMAKE_SOURCE_DIR}/include/common.h
                    ${CMAKE_SOURCE_DIR}/include/framebuffer.h
                    ${CMAKE_SOURCE_DIR}/include/init.h
                    ${CMAKE_SOURCE_DIR}/include/lights.h
                    ${CMAKE_SOURCE_DIR}/include/mat2.h
                    ${CMAKE_SOURCE_DIR}/include/mat3.h
                    ${CMAKE_SOURCE_DIR}/include/mat4.h
                    ${CMAKE_SOURCE_DIR}/include/material.h
                    ${CMAKE_SOURCE_DIR}/include/objLoader.h
                    ${CMAKE_SOURCE_DIR}/include/pixel.h
                    ${CMAKE_SOURCE_DIR}/include/quat.h
                    ${CMAKE_SOURCE_DIR}/include/rasterizer.h
                    ${CMAKE_SOURCE_DIR}/include/shapes.h
                    ${CMAKE_SOURCE_DIR}/include/transform.h
                    ${CMAKE_SOURCE_DIR}/include/cursedgl.h
                    ${CMAKE_SOURCE_DIR}/include/vec.h
                    ${CMAKE_SOURCE_DIR}/include/error.h
                    ${CMAKE_SOURCE_DIR}/include/texture.h
                    ${CMAKE_SOURCE_DIR}/tp/stb_image.h)

# include directories
include_directories (
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/tp"
)

# build library
set(LIBS ${LIBS} notcurses-core m pthread)
add_library(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# build examples

# colored_cube
set(EXAMPLE colored_cube)
add_executable(${EXAMPLE} examples/colored_cube.c)
target_link_libraries(${EXAMPLE} PUBLIC ${LIBS} ${PROJECT_NAME})

# interpolation
set(EXAMPLE interpolation)
add_executable(${EXAMPLE} examples/interpolation.c)
target_link_libraries(${EXAMPLE} PUBLIC ${LIBS} ${PROJECT_NAME})

# hello_triangle
set(EXAMPLE hello_triangle)
add_executable(${EXAMPLE} examples/hello_triangle.c)
target_link_libraries(${EXAMPLE} PUBLIC ${LIBS} ${PROJECT_NAME})

# hello_textured_triangle
set(EXAMPLE hello_textured_triangle)
add_executable(${EXAMPLE} examples/hello_textured_triangle.c)
target_link_libraries(${EXAMPLE} PUBLIC ${LIBS} ${PROJECT_NAME})

# teapot
set(EXAMPLE teapot)
add_executable(${EXAMPLE} examples/teapot.c)
target_link_libraries(${EXAMPLE} PUBLIC ${LIBS} ${PROJECT_NAME})

# montecarlo
set(EXAMPLE montecarlo)
add_executable(${EXAMPLE} examples/montecarlo.c)
target_link_libraries(${EXAMPLE} PUBLIC ${LIBS} ${PROJECT_NAME})

# hello_cube
set(EXAMPLE hello_cube)
add_executable(${EXAMPLE} examples/hello_cube.c)
target_link_libraries(${EXAMPLE} PUBLIC ${LIBS} ${PROJECT_NAME})

# first_person_demo
set(EXAMPLE first_person_demo)
add_executable(${EXAMPLE} examples/first_person_demo.c)
target_link_libraries(${EXAMPLE} PUBLIC ${LIBS} ${PROJECT_NAME})

# hello_lines
set(EXAMPLE hello_lines)
add_executable(${EXAMPLE} examples/hello_lines.c)
target_link_libraries(${EXAMPLE} PUBLIC ${LIBS} ${PROJECT_NAME})

# usage
set(EXAMPLE usage)
add_executable(${EXAMPLE} examples/usage.c)
target_link_libraries(${EXAMPLE} PUBLIC ${LIBS} ${PROJECT_NAME})
