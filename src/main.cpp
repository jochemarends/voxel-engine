#include <cstdlib>
#include <memory>
#include <print>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <utility/scope_guard.h>
#include <graphics/buffer.h>

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
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window.get());
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window.get())) {
        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }

    glfwDestroyWindow(window.get());
    glfwTerminate();
}

