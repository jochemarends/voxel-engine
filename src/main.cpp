#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
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
#include <utility/angle.h>
#include <utility/scope_guard.h>
#include <world/frustrum.h>
#include <world/cube.h>

glm::mat4 view{1.0f};

void cursor_pos_callback([[maybe_unused]] GLFWwindow* window, double x, double y) {
    constexpr float sensitivity{0.2f};
    static glm::dvec2 old{x, y};

    [[maybe_unused]] auto pitch = ja::degrees<float>((y - old.y) * sensitivity);
    [[maybe_unused]] auto heading = ja::degrees<float>(-(old.x - x) * sensitivity);

    old = {x, y};
}

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

    auto window = std::unique_ptr<GLFWwindow, window_deleter>{glfwCreateWindow(640, 480, "Hello Texture", nullptr, nullptr)};

    if (!window) {
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window.get());

    glfwSetFramebufferSizeCallback(window.get(), []([[maybe_unused]] GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    glfwSetCursorPosCallback(window.get(), cursor_pos_callback);

    glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwFocusWindow(window.get());

    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    const auto vertices = ja::cube_vertices() | std::ranges::to<std::vector>();
    using vertex_type = std::ranges::range_value_t<decltype(vertices)>;

    const auto indices = ja::cube_indices() | std::ranges::to<std::vector>();
    using index_type = std::ranges::range_value_t<decltype(indices)>;

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
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex_type), vertices.data(), GL_STATIC_DRAW);

    auto ebo = ja::make_buffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.get());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(index_type), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_type), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_type), reinterpret_cast<void*>(offsetof(vertex_type, texcoord)));
    glEnableVertexAttribArray(1);

    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    ja::frustrum frustrum{};

    {
        auto proj = glm::perspective(frustrum.fov.radians(), 640.0f / 480.0f, frustrum.near, frustrum.far);
        int location = glGetUniformLocation(program.get(), "proj");
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(proj));
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D_ARRAY);

    double prev_time = glfwGetTime();

    while (!glfwWindowShouldClose(window.get())) {
        glClearColor(0, 156.0 / 255.0, 130 / 255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const float curr_time = glfwGetTime();
        const float delta_time = curr_time - prev_time;
        prev_time = curr_time;
        
        constexpr float speed{2.0f};

        // handle input
        if (glfwGetKey(window.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window.get(), true);
        }

        glm::vec3 input{};
        if (glfwGetKey(window.get(), GLFW_KEY_W) == GLFW_PRESS) {
            input += glm::vec3{0.0f, 0.0f, 1.0f};
        }

        if (glfwGetKey(window.get(), GLFW_KEY_S) == GLFW_PRESS) {
            input += glm::vec3{0.0f, 0.0f, -1.0f};
        }

        if (glfwGetKey(window.get(), GLFW_KEY_A) == GLFW_PRESS) {
            input += glm::vec3{1.0f, 0.0f, 0.0f};
        }

        if (glfwGetKey(window.get(), GLFW_KEY_D) == GLFW_PRESS) {
            input += glm::vec3{-1.0f, 0.0f, 0.0f};
        }

        if (glfwGetKey(window.get(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            input += glm::vec3{0.0f, 1.0f, 0.0f};
        }

        if (glfwGetKey(window.get(), GLFW_KEY_SPACE) == GLFW_PRESS) {
            input += glm::vec3{0.0f, -1.0f, 0.0f};
        }

        if (glm::length(input) > 0.0f) {
            view = glm::translate(view, speed * delta_time * glm::normalize(input));
        }

        {
            static glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, ja::degrees(45.0f * static_cast<float>(delta_time)).radians(), glm::vec3{0.0f, 1.0f, 0.0f});
            int location = glGetUniformLocation(program.get(), "model");
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
        }

        {
            int location = glGetUniformLocation(program.get(), "view");
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(view));
        }

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }
}

