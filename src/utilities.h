#pragma once
#include "precomp.h"

#include <filesystem>

namespace veng {

    bool streq(gsl::czstring a, gsl::czstring b);
    std::vector<std::uint8_t> ReadFile(std::filesystem::path shader_path);
}