#pragma once

#include "config-utils/shared/config-utils.hpp"
#include <string>
#include <unordered_map>



DECLARE_CONFIG(ChromaConfig,

        CONFIG_VALUE(PrintEnvironmentEnhancementDebug, bool, "", false);
        CONFIG_VALUE(customColorEventsEnabled, bool, "Custom Color Events", true);
        CONFIG_VALUE(environmentEnhancementsEnabled, bool, "Environment Enhancements", true);


        CONFIG_INIT_FUNCTION(

                CONFIG_INIT_VALUE(PrintEnvironmentEnhancementDebug);
            CONFIG_INIT_VALUE(customColorEventsEnabled);
            CONFIG_INIT_VALUE(environmentEnhancementsEnabled);

            )
)
