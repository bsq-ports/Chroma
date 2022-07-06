#pragma once

#include <unordered_set>
#include <unordered_map>

#include "colorizer/LightColorizer.hpp"

#include "GlobalNamespace/ILightWithId.hpp"
#include "GlobalNamespace/LightWithIdManager.hpp"
#include "custom-json-data/shared/VList.h"
#include "lighting/LightIDTableManager.hpp"

namespace Chroma {
    struct LightIdRegisterer {
        static std::unordered_map<GlobalNamespace::ILightWithId*, int> RequestedIDs;
        static std::unordered_set<GlobalNamespace::ILightWithId*> NeedToRegister;
        static GlobalNamespace::LightWithIdManager* lightWithIdManager;

        static void Reset() {
            RequestedIDs.clear();
            NeedToRegister.clear();
            lightWithIdManager = nullptr;
        }

        static void SetRequestedId(GlobalNamespace::ILightWithId* lightWithId, int id)
        {
            RequestedIDs[lightWithId] = id;
        }

        static void MarkForTableRegister(GlobalNamespace::ILightWithId* lightWithId)
        {
            NeedToRegister.emplace(lightWithId);
        }

        static void ForceUnregister(GlobalNamespace::ILightWithId* lightWithId)
        {
            int lightId = lightWithId->get_lightId();
            auto lights = VList(lightWithIdManager->lights[lightId]);
            int index = -1;

            for (auto i = 0; i < lights.size(); i++) {
                auto const& n = lights[i];
                if (n == lightWithId) {
                    index = i;
                    break;
                }
            }

            lights[index] = nullptr; // TODO: handle null
            LightIDTableManager::UnregisterIndex(lightId, index);
            LightColorizer::CreateLightColorizerContractByLightID(lightId, [lightWithId](LightColorizer& n) {n._lightSwitchEventEffect->UnregisterLight(lightWithId);});
            lightWithId->__SetIsUnRegistered();
        }
    };
}