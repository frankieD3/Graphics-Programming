#include <GLFW/glfw3.h>
#include <glfw_monitor.h>
#include <precomp.h>

namespace veng {

    gsl::span<GLFWmonitor*> GetMonitors() {
        std::int32_t monitor_count = 0;
        GLFWmonitor** monitor_pointers = glfwGetMonitors(&monitor_count);
        return gsl::span<GLFWmonitor*>(monitor_pointers, monitor_count);
    }

    glm::ivec2 GetMonitorPosition(gsl::not_null<GLFWmonitor*> monitor) {
        glm::ivec2 monitor_position;
        glfwGetMonitorPos(monitor,
                        &monitor_position.x,
                        &monitor_position.y);
        return monitor_position;
    }

    glm::ivec2 GetMonitorWorkareaSize(gsl::not_null<GLFWmonitor*> monitor) {
        glm::ivec2 monitor_size;
        glfwGetMonitorWorkarea(monitor,
                      nullptr,
                      nullptr,
                      &monitor_size.x,
                      &monitor_size.y);
        return monitor_size;
    }


    void SetWindowPos(gsl::not_null<GLFWwindow*> window,
                      const glm::ivec2& window_new_position) {
        glfwSetWindowPos(window,
                         static_cast<int32_t>(window_new_position.x),
                         static_cast<int32_t>(window_new_position.y));
                       return;
    }

    void MoveWindowToMonitor(gsl::not_null<GLFWwindow*> window, gsl::not_null<GLFWmonitor*> monitor) {
        glm::ivec2 window_size;
        glfwGetWindowSize(window,
                          &window_size.x,
                          &window_size.y);

        glm::ivec2 window_new_position = GetMonitorPosition(monitor) +
                (GetMonitorWorkareaSize(monitor) / 2) - (window_size / 2);
        
        glfwSetWindowPos(window,
                         static_cast<int32_t>(window_new_position.x),
                         static_cast<int32_t>(window_new_position.y));

        return;
    }   

    void CenterWindowOnMonitor(GLFWwindow* window, GLFWmonitor* monitor) {
        MoveWindowToMonitor(window, monitor);
        return;
    }
}