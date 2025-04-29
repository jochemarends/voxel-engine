#include <cstdlib>
#include <memory>
#include <print>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <graphics/buffer.h>
#include <graphics/program.h>
#include <graphics/shader.h>
#include <graphics/texture.h>
#include <graphics/vertex_array.h>
#include <ranges>
#include <utility/angle.h>
#include <utility/scope_guard.h>
#include <world/frustrum.h>
#include <world/chunk.h>
#include <world/cube.h>

struct {
    glm::vec3 pos{};
    glm::vec3 forward{0.0f, 0.0f, 1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
} camera{};

void cursor_pos_callback([[maybe_unused]] GLFWwindow* window, double x, double y) {
    constexpr float sensitivity{0.2f};
    static glm::dvec2 old{x, y};

    auto yaw = ja::degrees<float>((old.x - x) * sensitivity);
    auto pitch = ja::degrees<float>(-(y - old.y) * sensitivity);

    auto rot = glm::rotate(glm::mat4{1.0f}, yaw.radians(), glm::vec3{0.0f, 1.0f, 0.0f});
    rot = glm::rotate(rot, pitch.radians(), glm::normalize(glm::cross(camera.forward, camera.up)));
    camera.forward = glm::normalize(glm::vec3{rot * glm::vec4{camera.forward, 0.0f}});
    camera.up = glm::normalize(glm::vec3{rot * glm::vec4{camera.up, 0.0f}});

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

    camera.pos = glm::vec3{0.0f, 0.0f, -3.0f};

    ja::frustrum frustrum{};

    {
        auto proj = glm::perspective(frustrum.fov.radians(), 640.0f / 480.0f, frustrum.near, frustrum.far);
        int location = glGetUniformLocation(program.get(), "proj");
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(proj));
    }

    ja::chunk<8, 4, 7> chunk{};

    constexpr int empty{-1};
    constexpr int grass{0};

    for (auto [i, j, k] : chunk.indices()) {
        constexpr int dirt{1};
        constexpr int brick{6};
        constexpr int orange{5};

        if (j == 0) {
            chunk[i, j, k] = dirt;
            continue;
        }

        if ((i >= 1 && i <= 6) && ((k >= 1 && k <= 5))) {
            if (j == 1 || j == 2) {
                if ((i > 1 && i < 6) && (k > 1 && k < 5)) {
                    continue;;
                }
                chunk[i, j, k] = brick;
            } else {
                chunk[i, j, k] = orange;
            }
        }
    }

    // plants
    chunk[2, 1, 0] = grass;
    chunk[5, 1, 0] = grass;

    // door
    chunk[4, 1, 1] = empty;
    chunk[4, 2, 1] = empty;
    chunk[3, 1, 1] = empty;
    chunk[3, 2, 1] = empty;

    chunk.rebuild();
    glBindVertexArray(chunk.vertex_array());

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
            input += glm::vec3{0.0f, -1.0f, 0.0f};
        }

        if (glfwGetKey(window.get(), GLFW_KEY_SPACE) == GLFW_PRESS) {
            input += glm::vec3{0.0f, 1.0f, 0.0f};
        }

        if (glm::length(input) > 0.0f) {
            glm::vec3 forward = glm::normalize(glm::vec3{camera.forward.x, 0.0f, camera.forward.z});
            glm::vec3 right = glm::normalize(glm::cross(camera.up, camera.forward));
            camera.pos += forward * speed * delta_time * glm::normalize(input).z;
            camera.pos += right * speed * delta_time * glm::normalize(input).x;
            camera.pos.y += speed * delta_time * glm::normalize(input).y;
        }

        {
            static glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, ja::degrees(45.0f * static_cast<float>(delta_time)).radians(), glm::vec3{0.0f, 1.0f, 0.0f});
            model = glm::mat4(1.0f);
            int location = glGetUniformLocation(program.get(), "model");
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
        }

        {
            glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.forward, camera.up);
            int location = glGetUniformLocation(program.get(), "view");
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(view));
        }

        glDrawElements(GL_TRIANGLES, chunk.index_count(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }
}

