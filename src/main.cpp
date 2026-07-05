#include "precomp.h"
#include "glfw_initialization.h"
#include "glfw_monitor.h"
#include "glfw_window.h"
#include "graphics.h"


std::int32_t main(std::int32_t argc, gsl::zstring* argv[]) {

    const veng::GLFWInitialization _glfw;

    veng::Window window("Vulkan Engine",
                        {800, 600});

    veng::Graphics graphics(&window);

    // Get the list of monitors and their count
    //
    gsl::span<GLFWmonitor*> monitors = veng::GetMonitors();

    if (monitors.size() > 1) {
        veng::MoveWindowToMonitor(window.GetHandle(), monitors[1]);
    }

    window.TryMoveToMonitor(1);

    
    // Main loop
    //
    while (!glfwWindowShouldClose(window.GetHandle())) {
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}