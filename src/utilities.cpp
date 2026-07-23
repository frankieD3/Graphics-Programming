#include "precomp.h"
#include "utilities.h"
#include <spdlog/spdlog.h>
#include <fstream>
namespace veng {

    bool streq(gsl::czstring a, gsl::czstring b) {
        return std::strcmp(a, b) == 0;

    }

    std::vector<std::uint8_t> ReadFile(std::filesystem::path shader_path) {

        const bool path_exists = std::filesystem::exists(shader_path);
        if (!path_exists) {
            spdlog::error("Shader file does not exist: {}", shader_path.string());
            return {};
        }

        if (!std::filesystem::is_regular_file(shader_path)) {
            spdlog::error("Shader path is not a regular file: {}", shader_path.string());
            return {};
        }

        std::ifstream file(shader_path, std::ios::binary);
        if (!file.is_open()) {
            spdlog::error("Failed to open shader file: {}", shader_path.string());
            return {};
        }

        const std::uint32_t file_size = std::filesystem::file_size(shader_path);
        std::vector<std::uint8_t> buffer(file_size);
        file.read(reinterpret_cast<char*>(buffer.data()), file_size);
        file.close();

        return buffer;
    }


}