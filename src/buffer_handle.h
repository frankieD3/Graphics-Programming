#pragma once


#include <vulkan/vulkan.h>
#include <gsl/gsl>

namespace veng {

  struct BufferHandle {
    VkBuffer buffer;
    VkDeviceMemory memory;

    
  };

} // namespace veng