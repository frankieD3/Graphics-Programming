#pragma once

#include "precomp.h"
#include "glfw_window.h"
#include <vulkan/vulkan.h>
#include <iostream>
#include <optional>

namespace veng {
    

    class Graphics final{
      public:
        Graphics(gsl::not_null<Window*> window);

        ~Graphics();


      private:

//
// private members

        VkInstance instance_ = VK_NULL_HANDLE;

        gsl::not_null<Window*> window_;

        bool validation_enabled_ = false;

        VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;

        VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;

        struct QueueFamilyIndices{
            std::optional<std::uint32_t> graphics_family = std::nullopt;
            std::optional<std::uint32_t> presentation_family = std::nullopt;

            bool IsValid() const {
                return graphics_family.has_value() /*&& presentation_family.has_value() */;
            }   
        };

        VkDevice logical_device_ = VK_NULL_HANDLE;

        VkQueue graphics_queue_ = VK_NULL_HANDLE;

// private methods
//

        void initializeVulkan();

        void CreateInstance();

        // Extension methods
        //
        static bool AreAllExtensionsSupported(const gsl::span<gsl::czstring>& extensions);

        static bool ExtensionMatchesName(gsl::czstring extension_name,
                                  const VkExtensionProperties& extension_properties);

        static bool IsExtensionSupported(gsl::span<VkExtensionProperties> extension_properties,
                 gsl::czstring extension_name);

        // Instance extensions
        //

        static gsl::span<gsl::czstring> GetSuggestedInstanceExtensions();

        static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();

        std::vector<gsl::czstring> GetRequiredInstanceExtensions();

        // Validation layers
        //
        static bool LayerMatchesName(gsl::czstring layer_name,
                   const VkLayerProperties& layer_properties);

        static bool IsLayerSupported(gsl::span<VkLayerProperties> layer_properties,
                   gsl::czstring layer_name);

        static std::vector<VkLayerProperties> GetSupportedValidationLayers();

        static bool AreAllValidationLayersSupported(gsl::span<gsl::czstring> validation_layers); 

        // Physical and logical device
        //
        void PickPhysicalDevice();

        std::vector<VkPhysicalDevice> GetAvailablePhysicalDevices(VkInstance instance);    

        bool IsDeviceSuitable(VkPhysicalDevice device);

        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

        void CreateLogicalDeviceandQueues();

        // Debug messenger
        //

        void SetupDebugMessenger();

    };

}   // namespace veng