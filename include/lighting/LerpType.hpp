#pragma once

namespace Chroma {
    enum struct LerpType {
        RGB,
        HSV
    };

    static LerpType LerpTypeFromString(std::string_view str) {
        std::string converted;
        // Convert string to lower case
        std::transform(str.begin(), str.end(), converted.begin(), ::tolower);

        if (converted == "HSV")
            return LerpType::HSV;

        return LerpType::RGB;
    }
}