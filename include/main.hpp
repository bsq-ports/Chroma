#pragma once

#include "modloader/shared/modloader.hpp"

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "ChromaConfig.hpp"

#include <string>

Configuration& getConfig();
Logger& getLogger();

inline const std::string modName = "Chroma";

static ModInfo modInfo;