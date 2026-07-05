#pragma once

#include "precomp.h"

namespace veng {
    
    struct GLFWInitialization {
      public:
        GLFWInitialization(); 

        ~GLFWInitialization();

// Delete copy constructor and copy assignment operator to prevent copying of the GLFWInitialization instance
        GLFWInitialization(const GLFWInitialization&) = delete;
        GLFWInitialization& operator=(const GLFWInitialization&) = delete;
    };
}