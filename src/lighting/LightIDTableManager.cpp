#include "main.hpp"

#include "lighting/LightIDTableManager.hpp"

#include "lighting/environments/BigMirrorEnvironment.hpp"
#include "lighting/environments/BTSEnvironment.hpp"
#include "lighting/environments/CrabRaveEnvironment.hpp"
#include "lighting/environments/DefaultEnvironment.hpp"
#include "lighting/environments/DragonsEnvironment.hpp"
#include "lighting/environments/FitBeatEnvironment.hpp"
#include "lighting/environments/GlassDesertEnvironment.hpp"
#include "lighting/environments/GreenDayEnvironment.hpp"
#include "lighting/environments/GreenDayGrenadeEnvironment.hpp"
#include "lighting/environments/KaleidoscopeEnvironment.hpp"
#include "lighting/environments/KDAEnvironment.hpp"
#include "lighting/environments/LinkinParkEnvironment.hpp"
#include "lighting/environments/MonstercatEnvironment.hpp"
#include "lighting/environments/NiceEnvironment.hpp"
#include "lighting/environments/OriginsEnvironment.hpp"
#include "lighting/environments/PanicEnvironment.hpp"
#include "lighting/environments/RocketEnvironment.hpp"
#include "lighting/environments/TimbalandEnvironment.hpp"
#include "lighting/environments/TriangleEnvironment.hpp"


// Macros are addicting but so bad
#define ENVIRONMENT_MACRO(DO) \
    DO(BigMirrorEnvironment) \
    DO(BTSEnvironment) \
    DO(CrabRaveEnvironment) \
    DO(DefaultEnvironment) \
    DO(DragonsEnvironment) \
    DO(FitBeatEnvironment) \
    DO(GlassDesertEnvironment) \
    DO(GreenDayEnvironment) \
    DO(GreenDayGrenadeEnvironment) \
    DO(KaleidoscopeEnvironment) \
    DO(KDAEnvironment) \
    DO(LinkinParkEnvironment) \
    DO(MonstercatEnvironment) \
    DO(NiceEnvironment) \
    DO(OriginsEnvironment) \
    DO(PanicEnvironment) \
    DO(RocketEnvironment) \
    DO(TimbalandEnvironment) \
    DO(TriangleEnvironment) \

using namespace Chroma;

#define STATIC_ENVIRONMENT(VAR) static VAR VAR##_static;
ENVIRONMENT_MACRO(STATIC_ENVIRONMENT)
#undef STATIC_ENVIRONMENT

static const std::unordered_map<std::string, Chroma::EnvironmentData&> ENVIRONMENTS {
    #define MAKE_ENVIRONMENT(VAR) {#VAR, VAR##_static},
        ENVIRONMENT_MACRO(MAKE_ENVIRONMENT)
    #undef MAKE_ENVIRONMENT
};


void Chroma::LightIDTableManager::InitTable() {
    for (auto data : ENVIRONMENTS) {
        getLogger().info("Initializing environment data for %s", data.first.c_str());
        lightIdTable[data.first] = data.second.getEnvironmentLights();
    }
}

void LightIDTableManager::SetEnvironment(std::string environmentName) {
    auto it = lightIdTable.find(environmentName);

    if (it == lightIdTable.end()) {
        getLogger().warning("Table not found for %s", environmentName.c_str());
        activeTable = std::nullopt;
    } else {
        activeTable = environmentName;
    }
}

std::optional<int> LightIDTableManager::GetActiveTableValue(int type, int id) {
    if (activeTable)
    {
        auto table = lightIdTable[activeTable.value()];

        auto it = table[type].find(id);

        if (it != table[type].end()) {
            getLogger().info("Found the light!");
            return it->second;
        } else {
            getLogger().warning("Unable to find value for type %d and id %d.", type, id);
        }
    }

    getLogger().warning("Return not found");
    return std::nullopt;
}
