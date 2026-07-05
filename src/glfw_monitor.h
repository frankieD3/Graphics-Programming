#pragma once

#include "precomp.h"

struct GLFWmonitor;

struct GLFWwindow;

namespace veng {

    gsl::span<GLFWmonitor*> GetMonitors();

    glm::ivec2 GetMonitorPosition(gsl::not_null<GLFWmonitor*> monitor);

    glm::ivec2 GetMonitorWorkareaSize(gsl::not_null<GLFWmonitor*> monitor);

    void MoveWindowToMonitor(gsl::not_null<GLFWwindow*> window,
                                                        gsl::not_null<GLFWmonitor*> monitor);

    void SetWindowPos(gsl::not_null<GLFWwindow*> window,
                      const glm::ivec2& window_new_position);

    void CenterWindowOnMonitor(gsl::not_null<GLFWwindow*> window,
                                             gsl::not_null<GLFWmonitor*> monitor);

}