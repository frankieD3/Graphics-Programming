#include "precomp.h"
#include <GLFW/glfw3.h>
#include <glfw_initialization.h>
#include "spdlog/spdlog.h"

namespace veng {

    void glfw_error_callback(std::int32_t error_code,
                             const char* description) 
    {
        spdlog::error("GLFW Error ({}): {}", error_code, description); 
           
    }

    
    GLFWInitialization::GLFWInitialization() {

        glfwSetErrorCallback(glfw_error_callback);

        if(glfwInit() == GLFW_FALSE) {
            //std::exit(EXIT_FAILURE);
        }
    }

    GLFWInitialization::~GLFWInitialization() {
        glfwTerminate();
    }
}   