#pragma once

#include "config-utils/shared/config-utils.hpp"
#include <string>
#include <unordered_map>



DECLARE_CONFIG(ChromaConfig,

        CONFIG_VALUE(customColorEventsEnabled, bool, "Custom Color Events", true);
        CONFIG_VALUE(lightshowModifier, bool, "Lightshow Modifier", false);
        CONFIG_VALUE(environmentEnhancementsEnabled, bool, "Environment Enhancements", true);
        CONFIG_VALUE(playersPlace, bool, "Players Place", false);
        CONFIG_VALUE(spectrograms, bool, "Spectrograms ", false);

        CONFIG_VALUE(backColumns, bool, "Back Columns", false);
        CONFIG_VALUE(buildings, bool, "Buildings ", false);

        CONFIG_INIT_FUNCTION(

            CONFIG_INIT_VALUE(customColorEventsEnabled);
            CONFIG_INIT_VALUE(lightshowModifier);
            CONFIG_INIT_VALUE(environmentEnhancementsEnabled);
            CONFIG_INIT_VALUE(playersPlace);
            CONFIG_INIT_VALUE(spectrograms);

            CONFIG_INIT_VALUE(backColumns);
            CONFIG_INIT_VALUE(buildings);

            )
)
