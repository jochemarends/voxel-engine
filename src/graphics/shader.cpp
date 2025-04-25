#include <graphics/shader.h>
#include <fstream>
#include <iterator>
#include <glad/gl.h>

namespace ja {

void shader_deleter::operator()(GLuint shader) const {
    glDeleteShader(shader);
}

shader_handle make_shader_from_file(GLenum type, const std::string& file_name) {
    std::ifstream ifs{file_name};
    if (!ifs) return shader_handle{0};

    ifs >> std::noskipws;
    std::string text{std::istream_iterator<char>{ifs}, std::istream_iterator<char>{}};

    return make_shader_from_text(type, text);
}

shader_handle make_shader_from_text(GLenum type, std::string_view source) {
    shader_handle shader{glCreateShader(type)};

    const char* data = source.data();
    auto size = static_cast<int>(source.size());

    glShaderSource(shader.get(), 1, &data, &size);
    glCompileShader(shader.get());

    return shader;
}

}

