#include "precomp.h"
#include "glfw_window.h"
#include "graphics.h"

namespace veng {
    
    
    Graphics::Graphics(gsl::not_null<Window*> window) : window_(window) {
        initializeVulkan();
    }

    Graphics::~Graphics() {
        if (instance_ != VK_NULL_HANDLE) {
        vkDestroyInstance(instance_,
                          nullptr);
        }
    }

    
    void Graphics::initializeVulkan() {
        CreateInstance();
    }

    void Graphics::CreateInstance() {

        //  | 1 | Get the required extensions from GLFW and the supported extensions from Vulkan
        //
        gsl::span<gsl::czstring> suggested_extensions = GetSuggestedInstanceExtensions();
        std::vector<VkExtensionProperties> supported_extensions = GetSupportedInstanceExtensions();
        if (!AreAllExtensionsSupported(suggested_extensions)) {
            std::exit(EXIT_FAILURE);
        }

        

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
        instance_create_info.enabledExtensionCount = static_cast<std::uint32_t>(suggested_extensions.size());
        instance_create_info.ppEnabledExtensionNames = suggested_extensions.data();
        instance_create_info.enabledLayerCount = 0;
        instance_create_info.ppEnabledLayerNames = nullptr;


        VkResult result = vkCreateInstance(&instance_create_info, 
                             nullptr,
                             &instance_); 
        if (result != VK_SUCCESS) {
            std::exit(EXIT_FAILURE);
        }
    }

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
            return std::any_of(
                supported_extensions.begin(), supported_extensions.end(),
                [extension_name](const VkExtensionProperties& extension_properties) {
                    return ExtensionMatchesName(extension_name, extension_properties);
                });
               };

        return std::all_of(extensions.begin(), extensions.end(), is_extension_supported);
    }

    bool Graphics::ExtensionMatchesName(
        gsl::czstring extension_name,
        const VkExtensionProperties &extension_properties) {
        return veng::streq(extension_name, extension_properties.extensionName);
    }

    bool Graphics::IsExtensionSupported( gsl::span<VkExtensionProperties> extension_properties,
                               gsl::czstring extension_name) {

        return std::any_of(
            extension_properties.begin(), extension_properties.end(),
            std::bind_front(ExtensionMatchesName, extension_name));
    }


}  // namespace veng 