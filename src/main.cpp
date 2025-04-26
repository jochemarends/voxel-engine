#include "glm/fwd.hpp"
#include "utility/angle.h"
#include <cstdlib>
#include <memory>
#include <print>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <graphics/buffer.h>
#include <graphics/program.h>
#include <graphics/shader.h>
#include <graphics/texture.h>
#include <graphics/vertex_array.h>
#include <utility/scope_guard.h>
#include <world/frustrum.h>

int main() {
    if (!glfwInit()) return EXIT_FAILURE;
    ja::scope_guard _{glfwTerminate};

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    struct window_deleter {
        void operator()(GLFWwindow* window) const {
            glfwDestroyWindow(window);
        }
    };

    auto window = std::unique_ptr<GLFWwindow, window_deleter>{glfwCreateWindow(640, 480, "Hello Triangle", nullptr, nullptr)};

    if (!window) {
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window.get());
    glfwSetFramebufferSizeCallback(window.get(), []([[maybe_unused]] GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    static const float vertices[]{
        // positions           // texture coordinates
         0.5f,  0.5f, 0.0f,    1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,    1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,    0.0f, 1.0f,
    };

    static const unsigned int indices[]{  
        0, 1, 3,
        1, 2, 3,
    };

    // TODO: find out if the lines below are really needed
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D_ARRAY);
    // glEnable(GL_CULL_FACE);
    // glFrontFace(GL_CW);

    auto vertex_shader = ja::make_shader_from_file(GL_VERTEX_SHADER, "res/simple.vert");

    auto fragment_shader = ja::make_shader_from_file(GL_FRAGMENT_SHADER, "res/simple.frag");

    auto program = ja::make_program(vertex_shader, fragment_shader);
    glUseProgram(program.get());

    auto texture = ja::make_texture_atlas_from_file(5, 5, "res/texture-atlas.png");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture.get());
    glUniform1i(glGetUniformLocation(program.get(), "texture"), 0);

    auto vao = ja::make_vertex_array();
    glBindVertexArray(vao.get());

    auto vbo = ja::make_buffer();
    glBindBuffer(GL_ARRAY_BUFFER, vbo.get());
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    auto ebo = ja::make_buffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.get());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0); 

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    {
        glm::mat4 model = glm::mat4(1.0f);
        int location = glGetUniformLocation(program.get(), "model");
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
    }

    {
        glm::mat4 view{1.0f};
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        int location = glGetUniformLocation(program.get(), "view");
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(view));
    }

    ja::frustrum frustrum{};

    {
        auto proj = glm::perspective(frustrum.fov.radians(), 640.0f / 480.0f, frustrum.near, frustrum.far);
        int location = glGetUniformLocation(program.get(), "proj");
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(proj));
    }


    while (!glfwWindowShouldClose(window.get())) {
        glClearColor(0, 156.0 / 255.0, 130 / 255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }
}

