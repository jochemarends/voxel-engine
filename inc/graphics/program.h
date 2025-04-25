#ifndef JA_PROGRAM_H
#define JA_PROGRAM_H

#include <glad/gl.h>
#include <utility/unique_resource.h>
#include <graphics/shader.h>

namespace ja {

/**
 * A functor used to free program handles.
 */
struct program_deleter {
    void operator()(GLuint program) const;
};

using program_handle = unique_resource<GLuint, program_deleter>;

/**
 * Creates a program given an arbitrary number of shaders.
 */
template<typename... Ts>
requires (std::same_as<shader_handle, Ts> && ...)
program_handle make_program(const Ts&... shaders) {
    program_handle program{glCreateProgram()};
    (glAttachShader(program.get(), shaders.get()), ...);
    glLinkProgram(program.get());
    (glDetachShader(program.get(), shaders.get()), ...);
    return program;
}

}

#endif

