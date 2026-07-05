#pragma once

#include "precomp.h"
#include "glfw_window.h"
#include <vulkan/vulkan.h>

namespace veng {
    class Graphics final{
      public:
        Graphics(gsl::not_null<Window*> window);

        ~Graphics();


      private:

        void initializeVulkan();

        void CreateInstance();

        gsl::span<gsl::czstring> GetSuggestedInstanceExtensions();

        static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();

        static bool AreAllExtensionsSupported(const gsl::span<gsl::czstring>& extensions);

        static bool ExtensionMatchesName(gsl::czstring extension_name,
                                  const VkExtensionProperties& extension_properties);

        bool IsExtensionSupported( gsl::span<VkExtensionProperties> extension_properties,
                               gsl::czstring extension_name);

//
// private members
        VkInstance instance_ = VK_NULL_HANDLE;

        gsl::not_null<Window*> window_;
    };

}   // namespace veng