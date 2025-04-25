#ifndef JA_SHADER_H
#define JA_SHADER_H

#include <string>
#include <string_view>
#include <glad/gl.h>
#include <utility/unique_resource.h>

namespace ja {

/**
 * A functor used to free shader handles.
 */
struct shader_deleter {
    void operator()(GLuint shader) const;
};

using shader_handle = unique_resource<GLuint, shader_deleter>;

/**
 * Create shader from file.
 *
 * @param type Type of shader to create.
 * @param path to shader source code file.
 */
shader_handle make_shader_from_file(GLenum type, const std::string& path);

/**
 * Create shader from text.
 *
 * @param type Type of shader to created.
 * @param source Source code of the shader program.
 */
shader_handle make_shader_from_text(GLenum type, const std::string_view source);

}

#endif

