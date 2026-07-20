#include "precomp.h"
#include "glfw_window.h"
#include <iostream>
#include "graphics.h"
#include <vulkan/vulkan.h>
#include "spdlog/spdlog.h"
#include <set>

#pragma region VK_FUNCTION_EXT_IMPL

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* info,
    const VkAllocationCallbacks* allocator,
    VkDebugUtilsMessengerEXT* debug_messenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT function =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    if (function != nullptr) {
        return function(instance, info, allocator, debug_messenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks* allocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT function =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (function != nullptr) {
        function(instance, debug_messenger, allocator);
    }
}

#pragma endregion  // VK_FUNCTION_EXT_IMPL
namespace veng {

#pragma region VALIDATION_LAYERS
    // Layer validation methods
    //


    static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
        void* p_user_data) {

        if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            spdlog::warn("Validation Validation: {}", p_callback_data->pMessage);
        }
        else {
            spdlog::error("Validation Error: {}", p_callback_data->pMessage);
        }

        return VK_FALSE;
    }

    static VkDebugUtilsMessengerCreateInfoEXT GetCreateMessengerInfo() {
        VkDebugUtilsMessengerCreateInfoEXT create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.pNext = nullptr;
        create_info.flags = 0;
        create_info.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        create_info.pfnUserCallback = ValidationCallback;
        create_info.pUserData = nullptr;

        return create_info;
    }


    bool Graphics::LayerMatchesName(gsl::czstring layer_name,
        const VkLayerProperties& layer_properties) {

        return veng::streq(layer_properties.layerName, layer_name);

    }



    bool Graphics::IsLayerSupported(gsl::span<VkLayerProperties> layers, gsl::czstring name) {
        return std::any_of(layers.begin(), layers.end(),
            std::bind_front(LayerMatchesName, name));
    }

    std::vector<VkLayerProperties> Graphics::GetSupportedValidationLayers() {
        std::uint32_t layer_count = 0;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        if (layer_count == 0) {
            return {};
        }

        std::vector<VkLayerProperties> layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, layers.data());
        return layers;

    }

    bool Graphics::AreAllValidationLayersSupported(gsl::span<gsl::czstring> validation_layers) {

        std::vector<VkLayerProperties> supported_layers = GetSupportedValidationLayers();

        auto is_layer_supported = [&supported_layers](gsl::czstring layer_name) {
            return IsLayerSupported(supported_layers, layer_name);
            };

        return std::all_of(validation_layers.begin(), validation_layers.end(), is_layer_supported);
    }

    void Graphics::SetupDebugMessenger() {

        VkDebugUtilsMessengerCreateInfoEXT info = GetCreateMessengerInfo();
        VkResult result = vkCreateDebugUtilsMessengerEXT(instance_,
            &info,
            nullptr,
            &debug_messenger_);
        if (result != VK_SUCCESS) {
            spdlog::error("Cannot create debug messenger");
            return;
        }
    }

#pragma endregion // VALIDATION_LAYERS

#pragma region INSTANCE_AND_EXTENSIONS

    gsl::span<gsl::czstring> Graphics::GetSuggestedInstanceExtensions() {
        std::uint32_t glfw_extension_count = 0;
        gsl::czstring* glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
        return { glfw_extensions, glfw_extension_count };
    }


    std::vector<VkExtensionProperties> Graphics::GetSupportedInstanceExtensions() {
        std::uint32_t glfw_extension_count = 0;

        vkEnumerateInstanceExtensionProperties(nullptr,
            &glfw_extension_count,
            nullptr);

        if (glfw_extension_count == 0) {
            return {};
        }

        std::vector<VkExtensionProperties> glfw_extensions(glfw_extension_count);
        vkEnumerateInstanceExtensionProperties(nullptr,
            &glfw_extension_count,
            glfw_extensions.data());
        return glfw_extensions;
    }

    bool Graphics::ExtensionMatchesName(gsl::czstring extension_name,
        const VkExtensionProperties& extension_properties) {
        return veng::streq(extension_name, extension_properties.extensionName);
    }

    bool Graphics::IsExtensionSupported(gsl::span<VkExtensionProperties> extension_properties,
        gsl::czstring extension_name) {

        return std::any_of(
            extension_properties.begin(), extension_properties.end(),
            std::bind_front(ExtensionMatchesName, extension_name));
    }

    std::vector<gsl::czstring> Graphics::GetRequiredInstanceExtensions() {

        gsl::span<gsl::czstring> suggested_extensions = GetSuggestedInstanceExtensions();
        std::vector<gsl::czstring> required_extensions(suggested_extensions.size());
        std::copy(suggested_extensions.begin(), suggested_extensions.end(), required_extensions.begin());

        if (validation_enabled_) {
            required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        if (!AreAllExtensionsSupported(required_extensions)) {
            std::exit(EXIT_FAILURE);
        }

        return required_extensions;
    }



#pragma endregion // INSTANCE_AND_EXTENSIONS

    // Constructor and destructor
    //
    Graphics::Graphics(gsl::not_null<Window*> window) : window_(window) {

#if !defined(NDEBUG)
        validation_enabled_ = true;
#endif

        initializeVulkan();
    }

    Graphics::~Graphics() {
        // Clean up Vulkan resources
        // Order of destruction:
        // 1. Logical device                                                                                                    
        if (logical_device_ != VK_NULL_HANDLE) {
            for (auto image_view : swap_chain_image_views_) {
                vkDestroyImageView(logical_device_,
                                   image_view,
                                   nullptr);
            }

            if (swap_chain_ != VK_NULL_HANDLE) {
                vkDestroySwapchainKHR(logical_device_,
                                      swap_chain_,
                                      nullptr);
            }
            vkDestroyDevice(logical_device_, nullptr);
        }
        // 2. Debug messenger
        if (instance_ != VK_NULL_HANDLE) {
            if (surface_ != VK_NULL_HANDLE) {
                vkDestroySurfaceKHR(instance_,
                    surface_,
                    nullptr);
            }
            if (debug_messenger_ != VK_NULL_HANDLE) {
                vkDestroyDebugUtilsMessengerEXT(instance_,
                    debug_messenger_,
                    nullptr);
            }
            // 3. Vulkan instance
            vkDestroyInstance(instance_,
                nullptr);
        }
    }

    // Private methods
    //

    // Initializes Vulkan by creating a Vulkan instance with the required extensions and 
    // validation layers (if enabled)
    // 

    void Graphics::initializeVulkan() {
        CreateInstance();
        if (validation_enabled_) {
            SetupDebugMessenger();
        }
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDeviceandQueues();
        CreateSwapChain();

    }

    //  Creates a Vulkan instance with the required extensions
    //  and validation layers (if enabled)
    //
    void Graphics::CreateInstance() {

        std::array<gsl::czstring, 1> validation_layers = {
            "VK_LAYER_KHRONOS_validation"
        };

        if (!AreAllValidationLayersSupported(validation_layers)) {
            validation_enabled_ = false;
        }

        //  | 1 | Get the required extensions from GLFW and the supported extensions from Vulkan
        //

        std::vector<gsl::czstring> required_extensions = GetRequiredInstanceExtensions();

        VkApplicationInfo app_info{};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pNext = nullptr;
        app_info.pApplicationName = "Vulkan Engine";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "VEng";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instance_create_info{};
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pNext = nullptr;
        instance_create_info.pApplicationInfo = &app_info;
        instance_create_info.enabledExtensionCount = static_cast<std::uint32_t>(required_extensions.size());
        instance_create_info.ppEnabledExtensionNames = required_extensions.data();
        instance_create_info.enabledLayerCount = 0;
        instance_create_info.ppEnabledLayerNames = nullptr;

        VkDebugUtilsMessengerCreateInfoEXT messenger_create_info = GetCreateMessengerInfo();
        if (validation_enabled_) {
            instance_create_info.enabledLayerCount = static_cast<std::uint32_t>(validation_layers.size());
            instance_create_info.ppEnabledLayerNames = validation_layers.data();
            instance_create_info.pNext = &messenger_create_info;
        }
        else {
            instance_create_info.enabledLayerCount = 0;
            instance_create_info.ppEnabledLayerNames = nullptr;
        }

        VkResult result = vkCreateInstance(&instance_create_info,
            nullptr,
            &instance_);
        if (result != VK_SUCCESS) {
            std::exit(EXIT_FAILURE);
        }
    }


#pragma region DEVICE_AND_QUEUES

    /// @brief Checks if all required instance extensions are supported by the Vulkan instance
    /// @param extensions list of required instance extensions
    /// @return true if all required instance extensions are supported, false otherwise
    bool Graphics::AreAllExtensionsSupported(gsl::span<gsl::czstring> extensions)
    {

        std::vector<VkExtensionProperties> supported_extensions = GetSupportedInstanceExtensions();


        return std::all_of(extensions.begin(), extensions.end(),
                    std::bind_front(IsExtensionSupported, supported_extensions));
    }

    /// @brief Retrieves the list of available device extensions for the given physical device
    /// @param device physical device to query
    /// @return vector of available device extensions
    std::vector<VkExtensionProperties> Graphics::GetDeviceAvailableExtensions(VkPhysicalDevice device)
    {
        std::uint32_t available_extension_count = 0;
        vkEnumerateDeviceExtensionProperties(device,
            nullptr,
            &available_extension_count,
            nullptr);

        if (available_extension_count == 0) {
            return {};
        }
        std::vector<VkExtensionProperties> available_extensions(available_extension_count);
        vkEnumerateDeviceExtensionProperties(device,
            nullptr,
            &available_extension_count,
            available_extensions.data());
        return available_extensions;
    }

    /// @brief Checks if a given extension is supported by the device
    /// @param extensions list of available extensions
    /// @param extension_name name of the extension to check
    /// @return true if the extension is supported, false otherwise
    ///
    bool IsExtensionSupported(const std::vector<VkExtensionProperties>& extensions,
                              gsl::czstring extension_name)
    {
        return std::any_of(extensions.begin(), extensions.end(), [name = extension_name](const VkExtensionProperties& extension) {
            return veng::streq(extension.extensionName, name);
            });
    }


    /// @brief Check if the device supports the required queue families (graphics, compute, etc.)
    /// @param device physical device to check
    /// @return true if the device supports the required queue families, false otherwise
    ///
    bool Graphics::IsDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = FindQueueFamilies(device);
        return indices.IsValid() &&
            AreAllDeviceExtensionsSupported(device) && QuerySwapChainSupport(device).IsValid();
    }

    /// @brief Checks if all required device extensions are supported
    /// @param device physical device to check
    /// @return true if all required device extensions are supported, false otherwise
    ///
    bool Graphics::AreAllDeviceExtensionsSupported(VkPhysicalDevice device)
    {
        std::vector<VkExtensionProperties> available_extensions = GetDeviceAvailableExtensions(device);

        return std::all_of(
            required_device_extensions_.begin(), required_device_extensions_.end(),
            std::bind_front(IsExtensionSupported, available_extensions));
    }

    void Graphics::PickPhysicalDevice()
    {
        /// PickPhysicalDevice picks a physical device that supports the required features and extensions
        /// It also checks if the device supports the required queue families (graphics, compute, etc.)
        ///

        std::vector<VkPhysicalDevice> devices = GetAvailablePhysicalDevices();

        if (devices.empty()) {
            spdlog::error("No physical devices that support Vulkan");
            std::exit(EXIT_FAILURE);
        }

        std::erase_if(devices, std::not_fn(std::bind_front(&Graphics::IsDeviceSuitable, this)));

        // score devices and pick the best one
        // For now we just pick the first one, but we could implement a scoring system to pick the best one

        physical_device_ = devices[0];

    }

    std::vector<VkPhysicalDevice> Graphics::GetAvailablePhysicalDevices()
    {
        /// GetAvailablePhysicalDevices returns the list of available physical devices for 
        /// a given Vulkan instance
        /// @return list of available physical devices
        ///
        std::uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(instance_,
            &device_count,
            nullptr);

        if (device_count == 0) {
            return {};
        }

        std::vector<VkPhysicalDevice> devices(device_count);

        vkEnumeratePhysicalDevices(instance_,
            &device_count,
            devices.data());

        return devices;

    }
    //////////////////////

    Graphics::QueueFamilyIndices Graphics::FindQueueFamilies(VkPhysicalDevice device)
    {
        std::uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device,
                                                 &queue_family_count,
                                                 nullptr);

        if (queue_family_count == 0) {
            return {};
        }

        std::vector<VkQueueFamilyProperties> families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device,
                                                 &queue_family_count,
                                                 families.data());

        auto graphics_family_it = std::find_if(families.begin(), families.end(),
            [](const VkQueueFamilyProperties& family) {
                return family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
            });

        if (graphics_family_it == families.end()) {
            return {};
        }

        QueueFamilyIndices indices;


        for (std::uint32_t i = 0; i < queue_family_count; ++i) {
            VkBool32 presentation_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentation_support);
            if (presentation_support) {
                indices.presentation_family = i;
                break;
            }
        }
        indices.graphics_family = static_cast<std::uint32_t>(std::distance(families.begin(), graphics_family_it));

        return indices;
    }

    /// @brief Creates a logical device and retrieves the graphics and presentation queues
    /// @details This function creates a logical device for the selected physical device and retrieves the graphics
    /// and presentation queues. It also checks if the required queue families are available.
    void Graphics::CreateLogicalDeviceandQueues() {

        QueueFamilyIndices picked_device_families = FindQueueFamilies(physical_device_);

        if (!picked_device_families.IsValid()) {
            spdlog::error("Physical device does not have the required queue families");
            std::exit(EXIT_FAILURE);
        }

        // Create a set of unique queue families to avoid creating multiple queues for the same family
        //
        std::set<std::uint32_t> unique_queue_families = {
            picked_device_families.graphics_family.value(),
            picked_device_families.presentation_family.value()
        };

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::float_t queue_priority = 1.0f;

        for (std::uint32_t unique_queue_family : unique_queue_families) {

            VkDeviceQueueCreateInfo queue_create_info{};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = unique_queue_family;
            queue_create_info.queueCount = 1;
            queue_create_info.pQueuePriorities = &queue_priority;
            queue_create_infos.push_back(queue_create_info);


        }


        VkPhysicalDeviceFeatures device_features{};

        VkDeviceCreateInfo device_create_info{};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.queueCreateInfoCount = queue_create_infos.size();
        device_create_info.pQueueCreateInfos = queue_create_infos.data();
        device_create_info.pEnabledFeatures = &device_features;
        device_create_info.enabledExtensionCount = required_device_extensions_.size();
        device_create_info.ppEnabledExtensionNames = required_device_extensions_.data();
        device_create_info.enabledLayerCount = 0;   // Deprecated in Vulkan 1.0, ignored in later versions

        VkResult result = vkCreateDevice(physical_device_,
            &device_create_info,
            nullptr,
            &logical_device_);
        if (result != VK_SUCCESS) {
            spdlog::error("Failed to create logical device");
            std::exit(EXIT_FAILURE);

        }

        vkGetDeviceQueue(logical_device_,
            picked_device_families.graphics_family.value(),
            0,
            &graphics_queue_);

        vkGetDeviceQueue(logical_device_,
            picked_device_families.presentation_family.value(),
            0,
            &presentation_queue_);


    }






#pragma endregion // DEVICE_AND_QUEUES

#pragma region PRESENTATION

    // Presentation methods
    //

    void Graphics::CreateSurface() {
        // Create a Vulkan surface for presentation using GLFW
        //
        if (glfwCreateWindowSurface(instance_,
            window_->GetHandle(),
            nullptr,
            &surface_) != VK_SUCCESS) {
            spdlog::error("Failed to create window surface");
            std::exit(EXIT_FAILURE);
        }
    }



    /// @brief Queries the swap chain support details for the given physical device.
    /// @param device The physical device to query.
    /// @return A SwapChainProperties structure containing the swap chain support details.
    Graphics::SwapChainProperties Graphics::QuerySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainProperties properties;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &properties.capabilities);

        std::uint32_t format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, nullptr);
        if (format_count > 0) {
            properties.formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, properties.formats.data());
        }

        std::uint32_t present_mode_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, nullptr);
        if (present_mode_count > 0) {
            properties.present_modes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, properties.present_modes.data());
        }

        return properties;
    }

    /// @brief Creates a swap chain for the selected physical device and surface.
    /// @details This function queries the swap chain support details for the selected physical device and surface
    /// and creates a swap chain with the best available surface format, present mode, and extent.
    /// @note This function assumes that the physical device and surface have already been selected and created.
    ///
    void Graphics::CreateSwapChain() {
        // Create a swap chain for the selected physical device and surface
        //
        SwapChainProperties swap_chain_properties = QuerySwapChainSupport(physical_device_);

        if (!swap_chain_properties.IsValid()) {
            spdlog::error("Physical device does not support swap chain");
            std::exit(EXIT_FAILURE);
        }

        // Choose the best surface format, present mode, and extent
        VkSurfaceFormatKHR surface_format_ = ChooseSwapSurfaceFormat(swap_chain_properties.formats);
        VkPresentModeKHR swap_chain_present_mode_ = ChooseSwapPresentMode(swap_chain_properties.present_modes);
        VkExtent2D swap_chain_extent_ = ChooseSwapExtent(swap_chain_properties.capabilities);
        std::uint32_t image_count = ChooseImageCount(swap_chain_properties.capabilities);

        // Setup the swap chain
        // Image count, 
        // format, 
        // color space, 
        // extent, 
        // image array layers, 
        // image usage, 
        // present mode, 
        // pre-transform, 
        // composite alpha, 
        // clipping, 
        // old swap chain
        //
        VkSwapchainCreateInfoKHR swap_chain_create_info{};
        swap_chain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swap_chain_create_info.surface = surface_;
        swap_chain_create_info.minImageCount = image_count;
        swap_chain_create_info.imageFormat = surface_format_.format;
        swap_chain_create_info.imageColorSpace = surface_format_.colorSpace;
        swap_chain_create_info.imageExtent = swap_chain_extent_;
        swap_chain_create_info.imageArrayLayers = 1;
        swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swap_chain_create_info.presentMode = swap_chain_present_mode_;
        swap_chain_create_info.preTransform = swap_chain_properties.capabilities.currentTransform;
        swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swap_chain_create_info.clipped = VK_TRUE;
        swap_chain_create_info.oldSwapchain = VK_NULL_HANDLE;

        // Setup the queue family indices for the swap chain sharing mode
        //
        QueueFamilyIndices indices = FindQueueFamilies(physical_device_);
        std::array<std::uint32_t, 2> queue_family_indices = {
            indices.graphics_family.value(),
            indices.presentation_family.value()
        };

        if (indices.graphics_family != indices.presentation_family) {
            // If the graphics and presentation families are different, use concurrent sharing mode
            swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swap_chain_create_info.queueFamilyIndexCount = queue_family_indices.size();
            swap_chain_create_info.pQueueFamilyIndices = queue_family_indices.data();
        }
        else {
            // If the graphics and presentation families are the same, use exclusive sharing mode
            swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swap_chain_create_info.queueFamilyIndexCount = 0;      // Optional
            swap_chain_create_info.queueFamilyIndexCount = 0;      // Optional
            swap_chain_create_info.pQueueFamilyIndices = nullptr; // Optional
        }


        if (vkCreateSwapchainKHR(logical_device_,
            &swap_chain_create_info,
            nullptr,
            &swap_chain_) != VK_SUCCESS)
        {
            spdlog::error("Failed to create swap chain");
            std::exit(EXIT_FAILURE);
        }

        swap_chain_images_.resize(image_count);

        // Retrieve the actual swap chain images from the Vulkan implementation
        // Due to AMD's driver behavior, the actual number of swap chain images may differ from the requested count.
        std::uint32_t actual_image_count = image_count;
        vkGetSwapchainImagesKHR(logical_device_,
                                swap_chain_,
                                &actual_image_count,
                                swap_chain_images_.data());
        swap_chain_images_.resize(actual_image_count);
        vkGetSwapchainImagesKHR(logical_device_,
                                swap_chain_,
                                &actual_image_count,
                                swap_chain_images_.data());


    }

    void Graphics::CreateImageViews() {
        swap_chain_image_views_.resize(swap_chain_images_.size());

        auto image_view_it = swap_chain_image_views_.begin();

        for (std::size_t i = 0; i < swap_chain_images_.size(); ++i) {
            VkImageViewCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = swap_chain_images_[i];
            create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format = surface_format_.format;
            create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(logical_device_,
                                                &create_info,
                                                nullptr,
                                                &image_view_it[i]);
            if (result != VK_SUCCESS) {
                spdlog::error("Failed to create image view for swap chain image {}", i);
                std::exit(EXIT_FAILURE);
            }
            std::next(image_view_it);
        }
    }

    // Helper functions for swap chain selection
    //
    std::uint32_t Graphics::ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        std::uint32_t image_count = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
            image_count = capabilities.maxImageCount;
        }
        return image_count;
    }

    bool IsRgbaTypeFormat(VkSurfaceFormatKHR format_properties)
    {

        return format_properties.format == VK_FORMAT_R8G8B8A8_SRGB ||
            format_properties.format == VK_FORMAT_B8G8R8A8_SRGB;

    }

    bool IsSrgbColorSpace(VkSurfaceFormatKHR format_properties)
    {
        return format_properties.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    }

    bool IsPreferredFormat(VkSurfaceFormatKHR format_properties)
    {
        return IsRgbaTypeFormat(format_properties) &&
            IsSrgbColorSpace(format_properties);
    }

    bool IsMailboxPresentMode(VkPresentModeKHR present_mode)
    {
        return present_mode == VK_PRESENT_MODE_MAILBOX_KHR;
    }

    /// @brief Chooses the best surface format from the available formats.
    /// @param available_formats A list of available surface formats.
    /// @return The chosen surface format.
    VkSurfaceFormatKHR Graphics::ChooseSwapSurfaceFormat(const std::span<VkSurfaceFormatKHR>& available_formats)
    {
        // Choose the best surface format from the available formats
        // Prefer VK_FORMAT_B8G8R8A8_SRGB with VK_COLOR_SPACE_SRGB_NONLINEAR_KHR

        auto it = std::find_if(available_formats.begin(), available_formats.end(),
            [](const VkSurfaceFormatKHR& format) {
                return IsPreferredFormat(format);
            });

        if (it != available_formats.end()) {
            return *it;
        }

        if (available_formats.size() == 1 && available_formats[0].format == VK_FORMAT_UNDEFINED) {
            return { VK_FORMAT_B8G8R8A8_SRGB,
                     VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        }

        // If the preferred format is not available, return the first available format
        return available_formats[0];
    }

    /// @brief Chooses the best present mode from the available present modes.
    /// @param available_present_modes A list of available present modes.
    /// @return The chosen present mode.
    ///
    VkPresentModeKHR Graphics::ChooseSwapPresentMode(const std::span<VkPresentModeKHR>& available_present_modes)
    {
        // Choose the best present mode from the available present modes
        // Prefer VK_PRESENT_MODE_MAILBOX_KHR for low latency and no tearing

        if (std::any_of(available_present_modes.begin(), available_present_modes.end(),
            IsMailboxPresentMode)) {
            return VK_PRESENT_MODE_MAILBOX_KHR;
        }

        // If the preferred present mode is not available, return VK_PRESENT_MODE_FIFO_KHR
        // which is guaranteed to be available
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    /// @brief Chooses the best swap extent (resolution) for the swap chain.
    /// @param capabilities The surface capabilities of the physical device.
    VkExtent2D Graphics::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        // Choose the best swap extent (resolution) for the swap chain
        //
        constexpr std::uint32_t kInvalidSize = std::numeric_limits<std::uint32_t>::max();
        if (capabilities.currentExtent.width != kInvalidSize) {
            return capabilities.currentExtent;
        }
        else
        {
            glm::ivec2 size = window_->GetFramebufferSize();


            VkExtent2D actual_extent = {
                static_cast<std::uint32_t>(size.x),
                static_cast<std::uint32_t>(size.y)
            };
            // Clamp the actual extent to the allowed range
            //
            actual_extent.width = std::clamp(actual_extent.width,
                                             capabilities.minImageExtent.width,
                                             capabilities.maxImageExtent.width);
            actual_extent.height = std::clamp(actual_extent.height,
                                             capabilities.minImageExtent.height,
                                             capabilities.maxImageExtent.height);

            return actual_extent;
        }
    }




#pragma endregion // PRESENTATION

}  // namespace veng 