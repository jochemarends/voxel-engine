#version 330 core
layout (location = 0) in vec3 pos_;
layout (location = 1) in vec2 texcoord_;

out vec3 texcoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    texcoord = vec3(texcoord_, 0);
    gl_Position = proj * view * model * vec4(pos_, 1.0);
} 

