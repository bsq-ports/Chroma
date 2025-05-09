#pragma once

// Forward declarations
class ChromaConfig;

#include <string>

// Only include what's needed in the header
#include "scotland2/shared/loader.hpp"

void setChromaEnv();

#if DEBUGB == 0
#define debugSpamLog(...) ChromaLogger::Logger.debug(__VA_ARGS__)
#else
#define debugSpamLog(...) ((void)0)
#endif

inline const std::string modName = "Chroma";

static modloader::ModInfo modInfo = {modName, VERSION, 0};