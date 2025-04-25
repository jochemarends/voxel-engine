#include <graphics/vertex_array.h>
#include "glad/gl.h"

namespace ja {

void vertex_array_deleter::operator()(GLuint vertex_array) const {
    glDeleteVertexArrays(1, &vertex_array);
}

vertex_array_handle make_vertex_array() {
    GLuint vertex_array{};
    glCreateVertexArrays(1, &vertex_array);
    return vertex_array_handle{vertex_array};
}

}
