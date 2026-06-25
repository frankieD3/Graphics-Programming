#pragma once

#include <GLFW/glfw3.h>

namespace veng {
    
    struct GLFWInitialization {
        GLFWInitialization(); 

        ~GLFWInitialization();

// Delete copy constructor and copy assignment operator to prevent copying of the GLFWInitialization instance
        GLFWInitialization(const GLFWInitialization&) = delete;
        GLFWInitialization& operator=(const GLFWInitialization&) = delete;
    };
}