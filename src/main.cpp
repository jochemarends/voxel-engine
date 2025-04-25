#include <cstdlib>
#include <functional>
#include <memory>
#include <print>
#include <string_view>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <graphics/buffer.h>
#include <graphics/program.h>
#include <graphics/shader.h>
#include <graphics/vertex_array.h>
#include <utility/scope_guard.h>

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

    auto window = std::unique_ptr<GLFWwindow, window_deleter>{glfwCreateWindow(640, 480, "", nullptr, nullptr)};

    if (!window) {
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window.get());
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

   const float vertices[]{
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    }; 

    std::string_view vertex_shader_source{
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0"
    };
    auto vertex_shader = ja::make_shader_from_text(GL_VERTEX_SHADER, vertex_shader_source);

    std::string_view fragment_shader_source{
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
            "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "} \n"
    };
    auto fragment_shader = ja::make_shader_from_text(GL_FRAGMENT_SHADER, fragment_shader_source);

    auto program = ja::make_program(vertex_shader, fragment_shader);
    glUseProgram(program.get());

    auto vao = ja::make_vertex_array();
    glBindVertexArray(vao.get());

    auto vbo = ja::make_buffer();
    glBindBuffer(GL_ARRAY_BUFFER, vbo.get());
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);  

    while (!glfwWindowShouldClose(window.get())) {
        glClearColor(0, 156.0 / 255.0, 130 / 255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }
}

