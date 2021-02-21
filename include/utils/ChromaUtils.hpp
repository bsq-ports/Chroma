#pragma once


#include "main.hpp"

#include "UnityEngine/Color.hpp"
#include "Chroma.hpp"

#include <optional>
#include <string>

namespace ChromaUtils {
    class ChromaUtilities {
    public:
        static bool IsModInstalled(std::string mod);

        static std::optional<UnityEngine::Color> GetColorFromData(rapidjson::Value* data, std::string member = Chroma::COLOR);

        // internal
        static void SetSongCoreCapability(std::string capability, bool enabled = true);
    };
}