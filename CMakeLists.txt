cmake_minimum_required(VERSION 3.22)
project(VulkanEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ensure debug symbols are generated for MSVC
if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
endif()

find_package(Vulkan REQUIRED)

include(cmake/Shaders.cmake)
include(FetchContent)

include(FetchContent)
fetchcontent_declare(
    glm
    GIT_REPOSITORY "https://github.com/g-truc/glm"
    GIT_TAG "1.0.3"
    GIT_SHALLOW TRUE)
fetchcontent_makeavailable(glm)

fetchcontent_declare(
    glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw"
    GIT_TAG "3.4"
    GIT_SHALLOW TRUE)
fetchcontent_makeavailable(glfw)

fetchcontent_declare(
    microsoft-gsl
    GIT_REPOSITORY "https://github.com/microsoft/gsl.git"
    GIT_TAG "v4.2.2"
    GIT_SHALLOW TRUE)
fetchcontent_makeavailable(microsoft-gsl)

fetchcontent_declare(
    spdlog
    GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
    GIT_TAG "v1.17.0"
    GIT_SHALLOW TRUE)
fetchcontent_makeavailable(spdlog)

file(GLOB_RECURSE VulkanEngineSources CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)
add_executable(VulkanEngine ${VulkanEngineSources})


target_link_libraries(VulkanEngine PRIVATE Vulkan::Vulkan)
target_link_libraries(VulkanEngine PRIVATE glm)
target_link_libraries(VulkanEngine PRIVATE glfw)
target_link_libraries(VulkanEngine PRIVATE Microsoft.GSL::GSL)
target_link_libraries(VulkanEngine PRIVATE spdlog)

target_include_directories(VulkanEngine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_compile_features(VulkanEngine PRIVATE cxx_std_20)

target_precompile_headers(VulkanEngine PRIVATE "${CMAKE_SOURCE_DIR}/src/precomp.h")

file(GLOB_RECURSE ShaderSources CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.vert"
	"${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.frag"
)

add_shaders(VulkanEngineShaders ${ShaderSources})
add_dependencies(VulkanEngine VulkanEngineShaders)