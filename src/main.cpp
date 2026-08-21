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

    std::array<veng::Vertex, 3> vertices = {
        veng::Vertex{glm::vec3{0.0f, -0.5f, 0.0f}, glm::vec3{1.0f, 0.0f, 0.0f}},
        veng::Vertex{glm::vec3{0.5f, 0.5f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f}},
        veng::Vertex{glm::vec3{-0.5f, 0.5f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f}}
    };

    veng::BufferHandle vertex_buffer = graphics.CreateVertexBuffer(vertices);

    std::array<std::uint32_t, 3> indices = { 0, 1, 2 };

    veng::BufferHandle index_buffer = graphics.CreateIndexBuffer(indices);

    
    // Main loop
    //
    while (!glfwWindowShouldClose(window.GetHandle())) {
        glfwPollEvents();
        if (graphics.BeginFrame()) {
            //graphics.RenderTriangle();
            //graphics.RenderBuffer(vertex_buffer,
            //                      static_cast<std::uint32_t>(vertices.size()));
            graphics.RenderIndexBuffer(vertex_buffer,
                                       index_buffer,
                                       static_cast<std::uint32_t>(indices.size()));
            graphics.EndFrame();
        }
    }

    vkDeviceWaitIdle(graphics.GetLogicalDevice());

    graphics.DestroyBuffer(vertex_buffer);
    graphics.DestroyBuffer(index_buffer);

    return EXIT_SUCCESS;
}