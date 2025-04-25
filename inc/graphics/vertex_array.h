#ifndef JA_VERTEX_ARRAY_H
#define JA_VERTEX_ARRAY_H

#include <glad/gl.h>
#include <utility/unique_resource.h>

namespace ja {

/**
 * A functor for freeing vertex array handles.
 */
struct vertex_array_deleter {
    void operator()(GLuint buffer) const;
};

using vertex_array_handle = unique_resource<GLuint, vertex_array_deleter>;

/**
 * Creates a new vertex array.
 */
vertex_array_handle make_vertex_array();

}

#endif

