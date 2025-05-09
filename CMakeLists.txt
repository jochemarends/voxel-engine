cmake_minimum_required(VERSION 3.22)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(voxel-engine LANGUAGES C CXX)

add_executable(app src/main.cpp)

target_compile_options(app PRIVATE -Werror -Wall -Wextra -pedantic)

target_compile_features(app PRIVATE cxx_std_26)

target_include_directories(app PRIVATE inc)

target_sources(app PRIVATE src/graphics/buffer.cpp src/graphics/vertex_array.cpp src/graphics/shader.cpp src/graphics/program.cpp src/graphics/texture.cpp)

target_sources(app PRIVATE src/world/cube.cpp)

configure_file(res/simple.vert res/simple.vert COPYONLY)
configure_file(res/simple.frag res/simple.frag COPYONLY)
configure_file(res/texture-atlas.png res/texture-atlas.png COPYONLY)

include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw 
    GIT_REPOSITORY https://github.com/glfw/glfw.git 
    GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

# glad
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    SOURCE_SUBDIR cmake
    GIT_TAG v2.0.8
)

FetchContent_MakeAvailable(glad)

glad_add_library(glad STATIC REPRODUCIBLE LOADER API gl:core=4.3)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
)

FetchContent_MakeAvailable(glm)

# stb
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
)

FetchContent_MakeAvailable(stb)

target_include_directories(app PRIVATE ${stb_SOURCE_DIR})

target_link_libraries(app PRIVATE glad glfw glm)

