#version 330 core

in vec3 texcoord;
out vec4 color;
uniform sampler2DArray textures;

void main() {
    // color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    color = texture(textures, texcoord);
}

