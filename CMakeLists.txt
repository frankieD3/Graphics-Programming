cmake_minimum_required(VERSION 3.22)
project(VulkanEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)

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

file(GLOB_RECURSE VulkanEngineSources CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)
add_executable(VulkanEngine ${VulkanEngineSources})


target_link_libraries(VulkanEngine PRIVATE Vulkan::Vulkan)
target_link_libraries(VulkanEngine PRIVATE glm)
target_link_libraries(VulkanEngine PRIVATE glfw)
target_link_libraries(VulkanEngine PRIVATE Microsoft.GSL::GSL)

target_include_directories(VulkanEngine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_compile_features(VulkanEngine PRIVATE cxx_std_20)

target_precompile_headers(VulkanEngine PRIVATE "${CMAKE_SOURCE_DIR}/src/precomp.h")