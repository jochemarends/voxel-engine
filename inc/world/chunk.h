#ifndef JA_CHUNK_H
#define JA_CHUNK_H

#include <cstddef>
#include <algorithm>
#include <print>
#include <ranges>
#include <vector>
#include <glad/gl.h>
#include <graphics/buffer.h>
#include <graphics/vertex_array.h>
#include <world/cube.h>

namespace ja {

template<std::size_t Width, std::size_t Height, std::size_t Depth>
struct chunk {
    static constexpr std::size_t width{Width};
    static constexpr std::size_t height{Height};
    static constexpr std::size_t depth{Depth};

    chunk() {
        auto data = std::views::join(std::views::join(data_));
        std::ranges::fill(data, -1);
    }

    void rebuild();

    [[nodiscard]] GLuint vertex_array() const { return vao_.get(); }
    [[nodiscard]] std::size_t index_count() const { return index_count_; }

    template<typename Self>
    auto&& operator[](this Self&& self, std::size_t i, std::size_t j, std::size_t k) {
        return self.data_[i][j][k];
    }
    [[nodiscard]] auto indices() const;
private:

    vertex_array_handle vao_{make_vertex_array()};
    buffer_handle vbo_{make_buffer()};
    buffer_handle ebo_{make_buffer()};
    std::size_t index_count_{};

    int data_[Width][Height][Depth]{};
};

template<std::size_t Width, std::size_t Height, std::size_t Depth>
void chunk<Width, Height, Depth>::rebuild() {
    std::vector<cube_vertex> vertices{};
    using vertex_type = std::ranges::range_value_t<decltype(vertices)>;
    std::vector<unsigned int> indices{};
    using index_type = std::ranges::range_value_t<decltype(indices)>;

    for (auto [index, block] : std::views::zip(this->indices(), std::views::join(std::views::join(data_)))) {
        if (block < 0) continue;
        auto [i, j, k] = index;
        
        const auto offset = vertices.size();

        std::ranges::copy(cube_vertices() | std::views::transform([block, i, j, k](vertex_type vertex) {
            vertex.position += glm::vec3{i, j, k};
            vertex.texcoord.z = block;
            return vertex;
        }), std::back_inserter(vertices));

        std::ranges::copy(cube_indices() | std::views::transform([offset](index_type index) {
            return index + offset;
        }), std::back_inserter(indices));
    }

    index_count_ = indices.size();

    glBindVertexArray(vao_.get());

    glBindBuffer(GL_ARRAY_BUFFER, vbo_.get());
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex_type), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_.get());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(index_type), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_type), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_type), reinterpret_cast<void*>(offsetof(vertex_type, texcoord)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

template<std::size_t Width, std::size_t Height, std::size_t Depth>
auto chunk<Width, Height, Depth>::indices() const {
    return std::views::cartesian_product(
        std::views::iota(0uz, width),
        std::views::iota(0uz, height),
        std::views::iota(0uz, depth)
    );
}

}

#endif

