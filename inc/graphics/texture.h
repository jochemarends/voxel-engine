#ifndef JA_TEXTURE_H
#define JA_TEXTURE_H

#include <string>
#include <glad/gl.h>
#include <utility/unique_resource.h>

namespace ja {

/**
 * A functor for freeing texture handles.
 */
struct texture_deleter {
    void operator()(GLuint texture) const;
};

using texture_handle = unique_resource<GLuint, texture_deleter>;

/**
 * Creates a new texture.
 */
texture_handle make_texture();


texture_handle make_texture_atlas_from_file(unsigned int rows, unsigned int columns, const std::string& path);

}

#endif

