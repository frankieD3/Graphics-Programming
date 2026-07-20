#include <GLFW/glfw3.h>
#include "glfw_window.h"
#include "glfw_monitor.h"
#include <gsl/gsl>

namespace veng {

    Window::Window(gsl::czstring title, glm::ivec2 size) {
        glfwWindowHint(GLFW_RESIZABLE,
                       GLFW_FALSE);
        glfwWindowHint(GLFW_CLIENT_API,
                       GLFW_NO_API);
        window_ = glfwCreateWindow(size.x,
                                   size.y,
                                   title,
                                   nullptr,
                                   nullptr);
        if (window_ == nullptr) {
            std::exit(EXIT_FAILURE);
        }
    }


    Window::~Window() {
        glfwDestroyWindow(window_);
    }

    GLFWwindow *Window::GetHandle() const
    {
        return window_;
    }

    /// @brief Gets the size of the window.
    /// @return The size of the window as a glm::ivec2.
    inline glm::ivec2 Window::GetWindowSize() const {
        glm::ivec2 window_size;
        glfwGetWindowSize(window_,
                      &window_size.x,
                      &window_size.y);
        return window_size;
    }

    /// @brief Gets the size of the framebuffer.
    /// @return The size of the framebuffer as a glm::ivec2.
    ///
    glm::ivec2 Window::GetFramebufferSize() const {
        glm::ivec2 framebuffer_size;
        glfwGetFramebufferSize(window_,
                               &framebuffer_size.x,
                               &framebuffer_size.y);
        return framebuffer_size;
    }

    /// @brief  Checks if the window should close.
    /// @return 
    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(window_);
    }

    /// @brief Tries to move the window to the specified monitor.
    /// @param monitor_index The index of the monitor to move the window to.
    /// @return True if the window was successfully moved, false otherwise.
    bool Window::TryMoveToMonitor(std::uint16_t monitor_index) {
        gsl::span<GLFWmonitor*> monitors = veng::GetMonitors();
        if (monitor_index < monitors.size()) {
            veng::MoveWindowToMonitor(window_, monitors[monitor_index]);
            return true;
        }
        return false;
    }



}   // namespace veng