#pragma once
#include <string_view>
#include <glm/vec2.hpp>
#include <gsl/gsl>
#include <precomp.h>

struct GLFWwindow;

namespace veng {
    
    class Window {

        public:
            Window(gsl::czstring title, glm::ivec2 size);

            ~Window();

            glm::ivec2 GetWindowSize() const;

            GLFWwindow* GetHandle() const;

            bool TryMoveToMonitor(std::uint16_t monitor_number);

            bool ShouldClose() const;


        private:
            GLFWwindow* window_;
    };

}   // namespace veng