#pragma once

#include "scotland2/shared/loader.hpp"

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "ChromaConfig.hpp"
#include "ChromaLogger.hpp"

#include <string>

void setChromaEnv();

#if DEBUGB == 0
#define debugSpamLog(...) ChromaLogger::Logger.debug(__VA_ARGS__)
#else
#define debugSpamLog(...) ((void)0)
#endif

inline const std::string modName = "Chroma";

static modloader::ModInfo modInfo = {modName, VERSION, 0};