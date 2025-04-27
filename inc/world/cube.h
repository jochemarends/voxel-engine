#ifndef JA_CUBE_H
#define JA_CUBE_H

#include <functional>
#include <ranges>
#include <span>
#include <glm/glm.hpp>

namespace ja {

enum class cube_face {
    front, back,
    left, right,
    top, bottom,
};

inline const std::array<cube_face, 6> cube_faces{
    cube_face::front, cube_face::back,
    cube_face::left, cube_face::right,
    cube_face::top, cube_face::bottom,
};

struct cube_vertex {
    glm::vec3 position{};
    glm::vec3 texcoord{};
};

inline const std::array<unsigned int, 6> cube_face_indices{0, 1, 2, 0, 2, 3};

[[nodiscard]] std::span<const cube_vertex, 4> cube_face_vertices(cube_face face);

[[nodiscard]] inline auto cube_vertices() {
    return std::views::transform(cube_faces, cube_face_vertices)
        | std::views::join;
}

[[nodiscard]] inline auto cube_indices() {
    return std::views::repeat(cube_face_indices, cube_faces.size())
        | std::views::enumerate
        | std::views::transform([](auto tuple) {
            auto [index, indices] = tuple;
            return std::views::transform(indices, std::bind_front(std::plus<unsigned int>{}, index * 4u));
        }) 
        | std::views::join;
}

}

#endif

