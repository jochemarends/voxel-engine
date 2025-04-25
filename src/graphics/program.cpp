#include <graphics/program.h>
#include <glad/gl.h>

namespace ja {

void program_deleter::operator()(GLuint program) const {
    glDeleteShader(program);
}

}

