#include "Chroma.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/typedefs-array.hpp"

#include "lighting/LightIDTableManager.hpp"
#include "colorizer/LightColorizer.hpp"
#include "utils/ChromaUtils.hpp"

#include <unordered_map>

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "UnityEngine/Mathf.hpp"

#include "GlobalNamespace/LightWithIdMonoBehaviour.hpp"
#include "GlobalNamespace/LightWithIds.hpp"
#include "GlobalNamespace/LightWithIds_LightWithId.hpp"
#include "GlobalNamespace/TrackLaneRingsManager.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"
#include "GlobalNamespace/ParticleSystemEventEffect.hpp"
#include "GlobalNamespace/ColorSO.hpp"

#include "System/Linq/Enumerable.hpp"


using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Sombrero;
using namespace Chroma;

LightColorizer::LightColorizer(ChromaLightSwitchEventEffect *lightSwitchEventEffect,
                               LightWithIdManager *lightManager)
                               : _lightSwitchEventEffect(lightSwitchEventEffect),
                                 lightId(lightSwitchEventEffect->lightsID),
                                 _colors(COLOR_FIELDS),
                                 _originalColors(),
                                 _simpleColorSOs(COLOR_FIELDS){
    static auto contextLogger = getLogger().WithContext(ChromaLogger::LightColorizer);


    // AAAAAA PROPAGATION STUFFF
    System::Collections::Generic::List_1<GlobalNamespace::ILightWithId *> *lightList = lightManager->lights.get(lightSwitchEventEffect->lightsID);
    ArrayW<ILightWithId *> lightArray = lightList->items;


    Lights = std::vector<ILightWithId *>();
    Lights.reserve(lightArray.Length());


    // Keep track of order
    int index = 0;
    std::unordered_map<int, int> insertionOrder;

    std::unordered_map<int, std::vector<ILightWithId *>> lightsPreGroup;

    auto managers = UnityEngine::Object::FindObjectsOfType<TrackLaneRingsManager *>();

    for (auto light : lightArray) {
        if (light == nullptr) continue;

        Lights.emplace_back(light);

        auto object = il2cpp_utils::cast<Il2CppObject>(light);
        auto monoBehaviour = il2cpp_utils::try_cast<MonoBehaviour>(object);

        if (!monoBehaviour) continue;

        int z1 = (int) std::round((double)(*monoBehaviour)->get_transform()->get_position().z);

        auto ring = (*monoBehaviour)->GetComponentInParent<TrackLaneRing *>();

        if (ring) {
            TrackLaneRingsManager *mngr = nullptr;
            auto indexR = 0;

            for (auto m : managers) {
                if (m) {
                    indexR = m->rings.IndexOf(ring);
                    if (indexR >= 0) {
                        mngr = m;
                        break;
                    }
                }
            }

            if (mngr != nullptr) { z1 = 1000 + indexR; }
        }

        debugSpamLog(contextLogger, "Grouping to %d", z1);

        std::vector<ILightWithId *> list;

        auto it = lightsPreGroup.find(z1);
        // Not found
        if (it == lightsPreGroup.end()) {
            insertionOrder[index] = z1;
            index++;
        } else list = it->second;

        list.push_back(light);

        lightsPreGroup.insert_or_assign(it, z1, list);
    }


    std::unordered_map<int, std::vector<ILightWithId *>> lightsPreGroupFinal;

    int i = 0;


    while (i <= index) {
        debugSpamLog(contextLogger, "Doing the final grouping, prop id %d", i);
        int z = insertionOrder[i];

        lightsPreGroupFinal[i] = lightsPreGroup[z];
        i++;
    }

    debugSpamLog(contextLogger, "Done grouping, size %d", lightsPreGroup.size());

    LightsPropagationGrouped = lightsPreGroupFinal;
}

LightColorizer & LightColorizer::New(ChromaLightSwitchEventEffect *lightSwitchEventEffect,
                                     GlobalNamespace::LightWithIdManager *lightManager) {
    CRASH_UNLESS(!Colorizers.contains(lightSwitchEventEffect->event.value));

    auto& light = Colorizers.try_emplace(lightSwitchEventEffect->event, lightSwitchEventEffect, lightManager).first->second;
    ColorizersByLightID.try_emplace(lightSwitchEventEffect->lightsID, &light);
    return light;
}



void LightColorizer::GlobalColorize(bool refresh, std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& lights, LightColorOptionalPalette const& colors) {
    for (int i = 0; i < colors.size(); i++)
    {
        GlobalColor[i] = colors[i];
    }

    for (auto& [_, colorizer] : Colorizers)
    {
        colorizer.SetSOs(colorizer.getColor());
        // Allow light colorizer to not force color
        if (!refresh)
        {
            continue;
        }

        colorizer.Refresh(lights);
    }
}

void LightColorizer::RegisterLight(UnityEngine::MonoBehaviour *lightWithId, std::optional<int> lightId) {
    auto const RegisterLightWithID = [&lightId](ILightWithId* lightToRegister) {
        int type = lightToRegister->get_lightId();
        if (type == -1) return;
        getLogger().debug("Registering light type %i", type);
        // TODO: Figure out -1 light ids
        auto lightColorizer = GetLightColorizerLightID(type);
        auto index = lightColorizer->Lights.size();
        LightIDTableManager::RegisterIndex(type, index, lightId);

        lightColorizer->_lightSwitchEventEffect->RegisterLight(lightToRegister, type, index);
        lightColorizer->Lights.emplace_back(lightToRegister);
    };

    auto monoBehaviourCast = il2cpp_utils::try_cast<LightWithIdMonoBehaviour>(lightWithId);

    if (monoBehaviourCast) {
        RegisterLightWithID(reinterpret_cast<ILightWithId *>(monoBehaviourCast.value()));
        return;
    }

    auto lightWithIdsCast = il2cpp_utils::try_cast<LightWithIds>(lightWithId);

    if (lightWithIdsCast) {
        auto lightWithIds = *lightWithIdsCast;
        auto lightsWithIdArray = System::Linq::Enumerable::ToArray(lightWithIds->lightWithIds);

        for (auto const& lightIdData : lightsWithIdArray) {
            RegisterLightWithID(il2cpp_utils::cast<ILightWithId>(lightIdData));
        }
    }
}

void LightColorizer::Reset() {
    for (int i = 0; i < COLOR_FIELDS; i++)
    {
        GlobalColor[i] = std::nullopt;
    }
    Colorizers.clear();
    ColorizersByLightID.clear();
    LightColorChanged.clear();
}

void LightColorizer::InitializeSO(ColorSO *&lightColor0, ColorSO *&highlightColor0, ColorSO *&lightColor1,
                                  ColorSO *&highlightColor1, ColorSO *&lightColor0Boost, ColorSO *&highlightColor0Boost,
                                  ColorSO *&lightColor1Boost, ColorSO *&highlightColor1Boost) {
    auto Initialize = [this](ColorSO*& colorSO, int index) {
        auto lightMultSO = il2cpp_utils::cast<MultipliedColorSO>(colorSO);

        Sombrero::FastColor multiplierColor = lightMultSO->multiplierColor;
        auto lightSO = lightMultSO->baseColor;
        _originalColors[index] = lightSO->color;

        SafePtr<MultipliedColorSO> mColorSO(ScriptableObject::CreateInstance<MultipliedColorSO *>());
        mColorSO->multiplierColor = multiplierColor;


        if (!_simpleColorSOs.contains(index)) {
            SafePtr<SimpleColorSO> sColorSO(ScriptableObject::CreateInstance<SimpleColorSO *>());
            sColorSO->SetColor(lightSO->color);
            _simpleColorSOs.emplace(index, sColorSO);
        }

        SafePtr<SimpleColorSO> &sColorSO = _simpleColorSOs[index];

        mColorSO->baseColor = (SimpleColorSO *) sColorSO;

        colorSO = (MultipliedColorSO*) mColorSO;
    };

    Initialize(lightColor0, 0);
    Initialize(highlightColor0, 0);
    Initialize(lightColor1, 1);
    Initialize(highlightColor1, 1);
    Initialize(lightColor0Boost, 2);
    Initialize(highlightColor0Boost, 2);
    Initialize(lightColor1Boost, 3);
    Initialize(highlightColor1Boost, 3);
}

std::vector<ILightWithId *> LightColorizer::GetPropagationLightWithIds(const std::vector<int> &ids) {
    std::vector<ILightWithId*> result;
    auto lightCount = LightsPropagationGrouped.size();
    for (int id : ids)
    {
        if (lightCount > id)
        {
            auto const& lights = LightsPropagationGrouped[id];

            for (auto light : lights) {
                result.push_back(light);
            }
        }
    }

    return result;
}

std::vector<ILightWithId *> LightColorizer::GetLightWithIds(std::vector<int> const &ids) {
    std::vector<ILightWithId*> result;
    result.reserve(ids.size());


    for (int id : ids)
    {
        // Transform
        id = LightIDTableManager::GetActiveTableValue(lightId, id).value_or(id);

        auto lightWithId = id >= 0 && id < Lights.size() ? Lights[id] : nullptr;
        if (lightWithId)
        {
            result.push_back(lightWithId);
        }
        else
        {
            getLogger().error("Type [%i] does not contain id [%i].", lightId, id);
        }
    }

    return result;
}

