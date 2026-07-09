#include "precomp.h"
#include "glfw_window.h"
#include <iostream>
#include "graphics.h"
#include <vulkan/vulkan.h>
#include "spdlog/spdlog.h"

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
  } else {
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
        } else {
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
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT ;
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

        VkDebugUtilsMessengerCreateInfoEXT info =  GetCreateMessengerInfo();
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
        return {glfw_extensions, glfw_extension_count};
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

    bool Graphics::AreAllExtensionsSupported(const gsl::span<gsl::czstring>& extensions) {


        std::vector<VkExtensionProperties> supported_extensions = GetSupportedInstanceExtensions();
        

        auto is_extension_supported = [&supported_extensions](gsl::czstring extension_name) {
            return IsExtensionSupported(supported_extensions, extension_name);
        };

        return std::all_of(extensions.begin(), extensions.end(), is_extension_supported);
    }
    bool Graphics::ExtensionMatchesName(gsl::czstring extension_name,
                                        const VkExtensionProperties &extension_properties) {
        return veng::streq(extension_name, extension_properties.extensionName);
    }

    bool Graphics::IsExtensionSupported( gsl::span<VkExtensionProperties> extension_properties,
                               gsl::czstring extension_name) {

        return std::any_of(
            extension_properties.begin(), extension_properties.end(),
            std::bind_front(ExtensionMatchesName, extension_name));
    }

    std::vector<gsl::czstring> Graphics::GetRequiredInstanceExtensions() {
        // Get the required extensions from GLFW 
        //
        gsl::span<gsl::czstring> suggested_extensions = GetSuggestedInstanceExtensions();
        std::vector<gsl::czstring> required_extensions(suggested_extensions.size());
        std::copy(suggested_extensions.begin(), suggested_extensions.end(), required_extensions.begin());

        // Add the debug utils extension if validation is enabled
        //
        if (validation_enabled_) {
            required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        // Check if all required extensions are supported
        //
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
        if (instance_ != VK_NULL_HANDLE) {
            if (debug_messenger_ != VK_NULL_HANDLE) {
                vkDestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
            }
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
    }

    //  Creates a Vulkan instance with the required extensions
    //  and validation layers (if enabled)
    //
    void Graphics::CreateInstance() {

        std::array<gsl::czstring, 1> validation_layers = {
            "VK_LAYER_KHRONOS_validation"
        };

        if(!AreAllValidationLayersSupported(validation_layers)) {
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
        if(validation_enabled_) {
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




}  // namespace veng 