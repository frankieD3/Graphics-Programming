#pragma once

#include "precomp.h"

struct GLFWwindow;

namespace veng {
    
    class Window {

        public:
            Window(gsl::czstring title, glm::ivec2 size);

            ~Window();

            GLFWwindow* GetHandle() const;

            bool TryMoveToMonitor(std::uint16_t monitor_number);

            bool ShouldClose() const;

            glm::ivec2 GetWindowSize() const;

            glm::ivec2 GetFramebufferSize() const;


        private:
            GLFWwindow* window_;
    };

}   // namespace veng