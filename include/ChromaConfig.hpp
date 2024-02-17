#pragma once

#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(ChromaConfig,

               CONFIG_VALUE(PrintEnvironmentEnhancementDebug, bool, "PrintEnvironmentEnhancementDebug", false);
               CONFIG_VALUE(customColorEventsEnabled, bool, "Custom Color Events", true,
                            "Disables custom colors for Chroma maps. In other words, disables Chroma.");
               CONFIG_VALUE(customNoteColors, bool, "Note coloring", true,
                            "Controls whether notes will be colored or not");
               CONFIG_VALUE(environmentEnhancementsEnabled, bool, "Environment Enhancements", true,
                            "Toggles whether Chroma should modify the environment as specified by a map.");
)