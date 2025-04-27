#include <world/cube.h>
#include <utility>

namespace ja {

namespace {

/**
 * Obtain the vertices of a cube_face.
 */
template<cube_face>
const std::array<cube_vertex, 4> vertices_of;

template<>
const std::array<cube_vertex, 4> vertices_of<cube_face::front>{
    cube_vertex{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 0.0f}},
    cube_vertex{{-0.5f, 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
    cube_vertex{{0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 0.0f}},
    cube_vertex{{0.5f,  -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
};

template<>
const std::array<cube_vertex, 4> vertices_of<cube_face::back>{
    cube_vertex{{0.5f,  -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    cube_vertex{{0.5f,  0.5f,  -0.5f}, {1.0f, 1.0f, 0.0f}},
    cube_vertex{{-0.5f, 0.5f,  -0.5f}, {0.0f, 1.0f, 0.0f}},
    cube_vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}},
};

template<>
const std::array<cube_vertex, 4> vertices_of<cube_face::left>{
    cube_vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}},
    cube_vertex{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    cube_vertex{{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 0.0f}},
    cube_vertex{{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
};

template<>
const std::array<cube_vertex, 4> vertices_of<cube_face::right>{
    cube_vertex{{0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
    cube_vertex{{0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 0.0f}},
    cube_vertex{{0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    cube_vertex{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}},
};

template<>
const std::array<cube_vertex, 4> vertices_of<cube_face::top>{
    cube_vertex{{-0.5f, 0.5f,  0.5f}, {0.0f, 0.0f, 0.0f}},
    cube_vertex{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    cube_vertex{{0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},
    cube_vertex{{0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
};

template<>
const std::array<cube_vertex, 4> vertices_of<cube_face::bottom>{
    cube_vertex{{0.5f,  -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
    cube_vertex{{0.5f,  -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},
    cube_vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    cube_vertex{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 0.0f}},
};

}

std::span<const cube_vertex, 4> cube_face_vertices(cube_face face) {
    switch(face) {
        case cube_face::front:
            return vertices_of<cube_face::front>;
        case cube_face::back:
            return vertices_of<cube_face::back>;
        case cube_face::left:
            return vertices_of<cube_face::left>;
        case cube_face::right:
            return vertices_of<cube_face::right>;
        case cube_face::top:
            return vertices_of<cube_face::top>;
        case cube_face::bottom:
            return vertices_of<cube_face::bottom>;
        default:
            std::unreachable();
    }
}

}
