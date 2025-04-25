#ifndef JA_BUFFER_H
#define JA_BUFFER_H

#include <glad/gl.h>
#include <utility/unique_resource.h>

namespace ja {

/**
 * A functor for freeing buffer handles.
 */
struct buffer_deleter {
    void operator()(GLuint buffer) const;
};

using buffer_handle = unique_resource<GLuint, buffer_deleter>;

/**
 * Creates a new buffer.
 */
buffer_handle make_buffer();

}

#endif

