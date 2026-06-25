#include <GLFW/glfw3.h>
#include "glfw_initialization.h"
#include <precomp.h>

#include <GLFW/glfw3.h>

namespace veng {
    
    GLFWInitialization::GLFWInitialization() {
        if(glfwInit() == GLFW_FALSE) {
            std::exit(EXIT_FAILURE);
        }
    }

    GLFWInitialization::~GLFWInitialization() {
        glfwTerminate();
    }
}   