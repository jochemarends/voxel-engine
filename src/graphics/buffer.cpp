#include <graphics/buffer.h>
#include "glad/gl.h"

namespace ja {

void buffer_deleter::operator()(GLuint buffer) const {
    glDeleteBuffers(1, &buffer);
}

buffer_handle make_buffer() {
    GLuint buffer{};
    glCreateBuffers(1, &buffer);
    return buffer_handle{buffer};
}

}

