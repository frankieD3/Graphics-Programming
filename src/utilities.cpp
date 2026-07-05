#include "precomp.h"
#include "utilities.h"

namespace veng {
    bool streq(gsl::czstring a, gsl::czstring b) {
        return std::strcmp(a, b) == 0;

    }
}


