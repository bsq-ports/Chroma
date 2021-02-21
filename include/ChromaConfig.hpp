#pragma once

#include "main.hpp"

#include <string>
#include <unordered_map>

#include "config-utils/shared/config-utils.hpp"



DECLARE_CONFIG(ChromaConfig,

        DECLARE_VALUE(customColorEventsEnabled, bool, "Custom Color Events", true);
        DECLARE_VALUE(lightshowModifier, bool, "Lightshow Modifier", false);
        DECLARE_VALUE(environmentEnhancementsEnabled, bool, "Environment Enhancements", true);
        DECLARE_VALUE(playersPlace, bool, "Players Place", false);
        DECLARE_VALUE(spectrograms, bool, "Spectrograms ", false);

        DECLARE_VALUE(backColumns, bool, "Back Columns", false);
        DECLARE_VALUE(buildings, bool, "Buildings ", false);

        INIT_FUNCTION(

            INIT_VALUE(customColorEventsEnabled);
            INIT_VALUE(lightshowModifier);
            INIT_VALUE(environmentEnhancementsEnabled);
            INIT_VALUE(playersPlace);
            INIT_VALUE(spectrograms);

            INIT_VALUE(backColumns);
            INIT_VALUE(buildings);

            )
)
