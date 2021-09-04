#pragma once

#include "config-utils/shared/config-utils.hpp"
#include <string>
#include <unordered_map>



DECLARE_CONFIG(ChromaConfig,

        CONFIG_VALUE(PrintEnvironmentEnhancementDebug, bool, "PrintEnvironmentEnhancementDebug", false);
        CONFIG_VALUE(customColorEventsEnabled, bool, "Custom Color Events", true, "Disables custom colors for Chroma maps. In other words, disables Chroma");
        CONFIG_VALUE(environmentEnhancementsEnabled, bool, "Environment Enhancements", true, "Toggles whether Chroma should modify the environment as specified by a map.");


        CONFIG_INIT_FUNCTION(

                CONFIG_INIT_VALUE(PrintEnvironmentEnhancementDebug);
            CONFIG_INIT_VALUE(customColorEventsEnabled);
            CONFIG_INIT_VALUE(environmentEnhancementsEnabled);

            )
)
