#include <graphics/texture.h>
#include <ranges>
#include <vector>
#include "glad/gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace ja {

void texture_deleter::operator()([[maybe_unused]] GLuint texture) const {
    glDeleteTextures(1, &texture);
}

texture_handle make_texture() {
    GLuint texture{};
    glGenTextures(1, &texture);
    return texture_handle{texture};
}

texture_handle make_texture_atlas_from_file(unsigned int rows, unsigned int columns, const std::string& path) {
    // stbi_set_flip_vertically_on_load(true);
    int width{}, height{}, channels{};
    auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (data == nullptr) {
        return texture_handle{0};
    }

    auto texture = make_texture();

    glBindTexture(GL_TEXTURE_2D_ARRAY, texture.get());

    // TODO: may let the caller be responsible for these parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    const int tile_width = width / columns;
    const int tile_height = height / rows;
    const int column_stride = tile_width * channels;
    const int format = (channels == 3) ? GL_RGB : GL_RGBA;

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, tile_width, tile_height, rows * columns, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    for (auto [row, column] : std::views::cartesian_product(std::views::iota(0u, rows), std::views::iota(0u, columns))) {
        const auto layer = static_cast<int>(row * columns + column);

        // extract tile from image
        auto buffer = std::span{data, static_cast<std::size_t>(width * height * channels)} 
            | std::views::chunk(column_stride)
            | std::views::drop(row * columns * tile_height + column)
            | std::views::stride(columns)
            | std::views::take(tile_height)
            | std::views::join
            | std::ranges::to<std::vector<unsigned char>>();

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, tile_width, tile_height, 1, format, GL_UNSIGNED_BYTE, buffer.data());
    }

    stbi_image_free(data);
    return texture;
}

}

