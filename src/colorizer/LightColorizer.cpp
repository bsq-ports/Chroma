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



    auto Initialize = [this](ColorSO*& colorSO, int index) {

        if (auto mColor = il2cpp_utils::try_cast<MultipliedColorSO>(colorSO)) {
            auto lightSO = mColor.value()->baseColor;
            _originalColors[index] = lightSO->color;
        } else if (auto sColor = il2cpp_utils::try_cast<SimpleColorSO>(colorSO)) {
            _originalColors[index] = sColor.value()->color;
        }
    };

    Initialize(lightSwitchEventEffect->lightColor0, 0);
    Initialize(lightSwitchEventEffect->lightColor1, 1);
    Initialize(lightSwitchEventEffect->lightColor0Boost, 2);
    Initialize(lightSwitchEventEffect->lightColor1Boost, 3);

    // AAAAAA PROPAGATION STUFFF
    Lights = lightManager->lights.get(lightSwitchEventEffect->lightsID);

    if(!Lights) {
        Lights = lightManager->lights.get(lightSwitchEventEffect->lightsID) = System::Collections::Generic::List_1<::GlobalNamespace::ILightWithId*>::New_ctor(10);
    }
}

LightColorizer & LightColorizer::New(ChromaLightSwitchEventEffect *lightSwitchEventEffect,
                                     GlobalNamespace::LightWithIdManager *lightManager) {
    auto& light = Colorizers.emplace(lightSwitchEventEffect->event, LightColorizer(lightSwitchEventEffect, lightManager)).first->second;
    ColorizersByLightID[lightSwitchEventEffect->lightsID] = &light;
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

void LightColorizer::Reset() {
    for (int i = 0; i < COLOR_FIELDS; i++)
    {
        GlobalColor[i] = std::nullopt;
    }
    Colorizers.clear();
    ColorizersByLightID.clear();
    LightColorChanged.clear();
    _contracts.clear();
    _contractsByLightID.clear();
    _contracts.shrink_to_fit();
    _contractsByLightID.shrink_to_fit();
}

void LightColorizer::InitializeSO(ColorSO *&lightColor0, ColorSO *&highlightColor0, ColorSO *&lightColor1,
                                  ColorSO *&highlightColor1, ColorSO *&lightColor0Boost, ColorSO *&highlightColor0Boost,
                                  ColorSO *&lightColor1Boost, ColorSO *&highlightColor1Boost) {
    auto Initialize = [this](ColorSO*& colorSO, int index) {

        if (auto mColor = il2cpp_utils::try_cast<MultipliedColorSO>(colorSO)) {
            auto lightSO = mColor.value()->baseColor;
            _originalColors[index] = lightSO->color;
        } else if (auto sColor = il2cpp_utils::try_cast<SimpleColorSO>(colorSO)) {
            _originalColors[index] = sColor.value()->color;
        }

//        auto lightMultSO = il2cpp_utils::cast<MultipliedColorSO>(colorSO);
//
//        Sombrero::FastColor multiplierColor = lightMultSO->multiplierColor;
//        auto lightSO = lightMultSO->baseColor;
//        _originalColors[index] = lightSO->color;
//
//        SafePtr<MultipliedColorSO> mColorSO(ScriptableObject::CreateInstance<MultipliedColorSO *>());
//        mColorSO->multiplierColor = multiplierColor;
//
//
//        if (!_simpleColorSOs.contains(index)) {
//            SafePtr<SimpleColorSO> sColorSO(ScriptableObject::CreateInstance<SimpleColorSO *>());
//            sColorSO->SetColor(lightSO->color);
//            _simpleColorSOs.emplace(index, sColorSO);
//        }
//
//        SafePtr<SimpleColorSO> &sColorSO = _simpleColorSOs[index];
//
//        mColorSO->baseColor = (SimpleColorSO *) sColorSO;
//
//        colorSO = (MultipliedColorSO*) mColorSO;
    };

    Initialize(lightColor0, 0);
//    Initialize(highlightColor0, 0);
    Initialize(lightColor1, 1);
//    Initialize(highlightColor1, 1);
    Initialize(lightColor0Boost, 2);
//    Initialize(highlightColor0Boost, 2);
    Initialize(lightColor1Boost, 3);
//    Initialize(highlightColor1Boost, 3);
}

std::vector<ILightWithId *> LightColorizer::GetPropagationLightWithIds(const std::vector<int> &ids) {
    std::vector<ILightWithId*> result;
    auto props = getLightsPropagationGrouped();
    auto lightCount = props.size();
    for (int id : ids)
    {
        if (lightCount > id)
        {
            auto const& lights = props.at(id);

            for (auto light : lights) {
                result.push_back(light);
            }
        }
    }

    return result;
}

std::vector<ILightWithId *> LightColorizer::GetLightWithIds(std::vector<int> const &ids) const {
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
    }

    return result;
}

void
LightColorizer::CreateLightColorizerContractByLightID(int lightId, std::function<void(LightColorizer &)> callback) {
    auto it = ColorizersByLightID.find(lightId);

    if (it != ColorizersByLightID.end())
    {
        CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Light id {} now", lightId);
        callback(*it->second);
    }
    else
    {
        CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Fulfilling callback for light id {} later", lightId);
        _contractsByLightID.emplace_back(lightId, callback);
    }
}

void LightColorizer::CreateLightColorizerContract(BasicBeatmapEventType type,
                                                  std::function<void(LightColorizer &)> callback) {
    auto it = Colorizers.find(type);

    if (it != Colorizers.end())
    {
        callback(it->second);
    }
    else
    {
        _contracts.emplace_back(type, callback);
    }
}

void LightColorizer::CompleteContracts(ChromaLightSwitchEventEffect* chromaLightSwitchEventEffect) {
    // complete open contracts
    for (auto it = _contracts.begin(); it != _contracts.end(); ) {
        auto const& [type, callback] = *it;
        if (type != chromaLightSwitchEventEffect->EventType) {
            it++;
            continue;
        }

        callback(*chromaLightSwitchEventEffect->lightColorizer);
        it = _contracts.erase(it);
    }

    for (auto it = _contractsByLightID.begin(); it != _contractsByLightID.end(); ) {
        auto const& [lightId, callback] = *it;
        if (lightId != chromaLightSwitchEventEffect->lightsID) {
            it++;
            continue;
        }

        callback(*chromaLightSwitchEventEffect->lightColorizer);
        it = _contractsByLightID.erase(it);
    }
}

std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>> LightColorizer::getLightsPropagationGrouped() {
    if (!LightsPropagationGrouped) {
        return *LightsPropagationGrouped;
    }
    // Keep track of order
    int index = 0;
    std::unordered_map<int, int> insertionOrder;

    std::unordered_map<int, std::vector<ILightWithId *>> lightsPreGroup;

    auto managers = UnityEngine::Object::FindObjectsOfType<TrackLaneRingsManager *>();

    for (auto light : Lights) {
        if (light == nullptr) continue;

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
