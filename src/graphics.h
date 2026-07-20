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

        VkSwapchainKHR swap_chain_ = VK_NULL_HANDLE;
        struct QueueFamilyIndices{
            std::optional<std::uint32_t> graphics_family = std::nullopt;
            std::optional<std::uint32_t> presentation_family = std::nullopt;

            bool IsValid() const {
              return graphics_family.has_value() && presentation_family.has_value();
            }

        };

        struct SwapChainProperties {
          VkSurfaceCapabilitiesKHR capabilities;
          std::vector<VkSurfaceFormatKHR> formats;
          std::vector<VkPresentModeKHR> present_modes;

          bool IsValid() const {
            return !formats.empty() && !present_modes.empty();
          }
        };

        std::array<gsl::czstring, 1> required_device_extensions_ = {

          VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        VkDevice logical_device_ = VK_NULL_HANDLE;

        VkSurfaceKHR surface_ = VK_NULL_HANDLE;

        VkQueue graphics_queue_ = VK_NULL_HANDLE;

        VkQueue presentation_queue_ = VK_NULL_HANDLE;

        std::vector<VkImage> swap_chain_images_;

        std::vector<VkImageView> swap_chain_image_views_;

        VkSurfaceFormatKHR surface_format_;
        VkFormat swap_chain_image_format_;
        VkExtent2D swap_chain_extent_;
        VkPresentModeKHR swap_chain_present_mode_;


// private methods
//

        void initializeVulkan();

        void CreateInstance();

        // Extension methods
        //

        /// @brief Checks if all the specified extensions are supported by the Vulkan instance.
        /// @param extensions A span of C-style strings representing the extensions to check.
        /// @return true if all extensions are supported, false otherwise.
        static bool AreAllExtensionsSupported(gsl::span<gsl::czstring> extensions);

        static bool ExtensionMatchesName(gsl::czstring extension_name,
                                  const VkExtensionProperties& extension_properties);

        static bool IsExtensionSupported(gsl::span<VkExtensionProperties> extension_properties,
                 gsl::czstring extension_name);

        // Instance extensions
        //

        /// @brief Get the suggested instance extensions required by GLFW.
        /// @return A span of C-style strings representing the suggested instance extensions.
        static gsl::span<gsl::czstring> GetSuggestedInstanceExtensions();

        static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions();

        std::vector<gsl::czstring> GetRequiredInstanceExtensions();

        std::vector<VkExtensionProperties> GetDeviceAvailableExtensions(VkPhysicalDevice device);

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

        bool AreAllDeviceExtensionsSupported(VkPhysicalDevice device);

        std::vector<VkPhysicalDevice> GetAvailablePhysicalDevices();

        bool IsDeviceSuitable(VkPhysicalDevice device);

        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

        void CreateLogicalDeviceandQueues();

        void CreateSurface();

        // Swap chain
        //

        /// @brief Queries the swap chain support details for the given physical device.
        /// @param device The physical device to query.
        /// @return A SwapChainProperties structure containing the swap chain support details.
        SwapChainProperties QuerySwapChainSupport(VkPhysicalDevice device);

        void CreateSwapChain();

        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::span<VkSurfaceFormatKHR>& available_formats);

        VkPresentModeKHR ChooseSwapPresentMode(const std::span<VkPresentModeKHR>& available_present_modes);

        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        std::uint32_t ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities);

        void CreateImageViews();


        void SetupDebugMessenger();

    };

}   // namespace veng