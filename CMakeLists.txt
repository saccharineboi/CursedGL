cmake_minimum_required(VERSION 3.10)
project(cursedgl)

# C settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

list(APPEND CMAKE_C_FLAGS "-Wall \
                           -Wextra \
                           -Werror \
                           -Og \
                           -g \
                           -flto \
                           -Wfloat-equal \
                           -Wundef \
                           -Wshadow \
                           -Wconversion \
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
                    ${CMAKE_SOURCE_DIR}/src/transform.c)

# add header files
set(HEADER_FILES    ${CMAKE_SOURCE_DIR}/src/common.h
                    ${CMAKE_SOURCE_DIR}/src/framebuffer.h
                    ${CMAKE_SOURCE_DIR}/src/init.h
                    ${CMAKE_SOURCE_DIR}/src/lights.h
                    ${CMAKE_SOURCE_DIR}/src/mat2.h
                    ${CMAKE_SOURCE_DIR}/src/mat3.h
                    ${CMAKE_SOURCE_DIR}/src/mat4.h
                    ${CMAKE_SOURCE_DIR}/src/material.h
                    ${CMAKE_SOURCE_DIR}/src/objLoader.h
                    ${CMAKE_SOURCE_DIR}/src/pixel.h
                    ${CMAKE_SOURCE_DIR}/src/quat.h
                    ${CMAKE_SOURCE_DIR}/src/rasterizer.h
                    ${CMAKE_SOURCE_DIR}/src/shapes.h
                    ${CMAKE_SOURCE_DIR}/src/transform.h
                    ${CMAKE_SOURCE_DIR}/src/cursedgl.h
                    ${CMAKE_SOURCE_DIR}/src/vec.h)

# include directories
include_directories (
    "${CMAKE_SOURCE_DIR}/src"
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
