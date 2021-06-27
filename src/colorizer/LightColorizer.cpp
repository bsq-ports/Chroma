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
#include "GlobalNamespace/LightWithIds_LightData.hpp"
#include "GlobalNamespace/TrackLaneRingsManager.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"
#include "GlobalNamespace/ParticleSystemEventEffect.hpp"
#include "GlobalNamespace/ColorSO.hpp"



using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

using namespace Chroma;

LightColorizer::LightColorizer(GlobalNamespace::LightSwitchEventEffect *lightSwitchEventEffect,
                               GlobalNamespace::BeatmapEventType beatmapEventType) {
//        INVOKE_CTOR(LSEColorManager);
    static auto contextLogger = getLogger().WithContext(ChromaLogger::LightColorizer);

    _lightSwitchEventEffect = lightSwitchEventEffect;
    _eventType = beatmapEventType;
    InitializeSO("_lightColor0", 0);
    InitializeSO("_highlightColor0", 0);
    InitializeSO("_lightColor1", 1);
    InitializeSO("_highlightColor1", 1);
    InitializeSO("_lightColor0Boost", 2);
    InitializeSO("_highlightColor0Boost", 2);
    InitializeSO("_lightColor1Boost", 3);
    InitializeSO("_highlightColor1Boost", 3);

    // AAAAAA PROPAGATION STUFFF

        auto lightManager = lightSwitchEventEffect->lightManager;
        System::Collections::Generic::List_1<GlobalNamespace::ILightWithId *> *lightList = lightManager->lights->get((lightSwitchEventEffect)->lightsID);
        Array<ILightWithId *> *lightArray = lightList->items;


        Lights = std::unordered_map<int, ILightWithId*>(lightArray->Length());


        // Keep track of order
        int index = 0;
        std::unordered_map<int, int> insertionOrder;

        std::unordered_map<int, std::vector<ILightWithId *>> lightsPreGroup;

        auto managers = UnityEngine::Object::FindObjectsOfType<TrackLaneRingsManager*>();

        for (int i = 0; i < lightArray->Length(); i++) {
            auto light = lightArray->get(i);
            if (light == nullptr) continue;

            debugSpamLog(contextLogger, "Adding light to list");
            Lights[i] = light;

            auto object = il2cpp_utils::cast<Il2CppObject>(light);
            debugSpamLog(contextLogger, "Doing light %s", object->klass->name);
            auto monoBehaviour = il2cpp_utils::try_cast<MonoBehaviour>(object);

            if (monoBehaviour) {

                int z = UnityEngine::Mathf::RoundToInt((*monoBehaviour)->get_transform()->get_position().z);

                auto ring = (*monoBehaviour)->GetComponentInParent<TrackLaneRing*>();

                if (ring) {

                    TrackLaneRingsManager* mngr = nullptr;
                    auto indexR = 0;

                    for (int ii = 0; ii < managers->Length(); ii++) {
                        auto m = managers->get(ii);

                        if (m) {
                            indexR = m->rings->IndexOf(ring);
                            if (indexR >= 0) {
                                mngr = m;
                                break;
                            }
                        }
                    }

                    if (mngr != nullptr)
                        z = 1000 + indexR;

                }

                debugSpamLog(contextLogger, "Grouping to %d", z);

                std::vector<ILightWithId *> list;

                auto it = lightsPreGroup.find(z);
                // Not found
                if (it == lightsPreGroup.end()) {
                    insertionOrder[index] = z;
                    index++;
                } else list = it->second;

                list.push_back(light);

                lightsPreGroup.insert_or_assign(it, z, list);
            }
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

std::shared_ptr<LightColorizer> LightColorizer::New(GlobalNamespace::LightSwitchEventEffect *lightSwitchEventEffect,GlobalNamespace::BeatmapEventType beatmapEventType) {
    std::shared_ptr<LightColorizer> lightColorizer = std::shared_ptr<LightColorizer>(new LightColorizer(lightSwitchEventEffect, beatmapEventType));

    Colorizers[beatmapEventType.value] = lightColorizer;
    return lightColorizer;
}

std::vector<UnityEngine::Color> LightColorizer::getColor() const {
    std::vector<UnityEngine::Color> colors(COLOR_FIELDS);
    for (int i = 0; i < COLOR_FIELDS; i++)
    {
        auto color = _colors[i];
        if (color) {
            colors[i] = *color;
            continue;
        }

        color = GlobalColor[i];
        if (color) {
            colors[i] = *color;
            continue;
        }

        colors[i] = _originalColors[i];
    }

    return colors;
}

void LightColorizer::GlobalColorize(bool refresh, std::vector<std::optional<UnityEngine::Color>> colors) {
    for (int i = 0; i < colors.size(); i++)
    {
        GlobalColor[i] = colors[i];
    }

    for (auto& valuePair : Colorizers)
    {
        // Allow light colorizer to not force color
        if (refresh)
        {
            valuePair.second->Refresh();
        }
        else
        {
            valuePair.second->SetSOs(valuePair.second->getColor());
        }
    }
}

void LightColorizer::RegisterLight(UnityEngine::MonoBehaviour *lightWithId, std::optional<int> lightId) {
    std::shared_ptr<LightColorizer> lightColorizer;

    auto monoBehaviourCast = il2cpp_utils::try_cast<LightWithIdMonoBehaviour>(lightWithId);;

    if (monoBehaviourCast) {
        auto monoBehaviour = *monoBehaviourCast;
        lightColorizer = GetLightColorizer((monoBehaviour->get_lightId() - 1));
        LightIDTableManager::RegisterIndex(monoBehaviour->get_lightId() - 1, lightColorizer->Lights.size(), lightId);
        lightColorizer->Lights[(int) lightColorizer->Lights.size()] = reinterpret_cast<ILightWithId *>(monoBehaviour);
        return;
    }

    auto lightWithIdsCast = il2cpp_utils::try_cast<LightWithIds>(lightWithId);

    if (lightWithIdsCast) {
        auto lightWithIds = *lightWithIdsCast;
        auto* lightsWithIdArray = il2cpp_utils::cast<Array<LightWithIds::LightData*>>(lightWithIds->get_lightIntensityData());


        for (int i = 0; i < lightsWithIdArray->Length(); i++) {
            auto light = il2cpp_utils::cast<ILightWithId>(lightsWithIdArray->get(i));
            lightColorizer = GetLightColorizer((light->get_lightId() - 1));

            LightIDTableManager::RegisterIndex(light->get_lightId() - 1, (int) lightColorizer->Lights.size(), lightId);
            lightColorizer->Lights[(int) lightColorizer->Lights.size()] = light;
        }
    }
}

void LightColorizer::Colorize(bool refresh, std::vector<std::optional<UnityEngine::Color>> colors) {
    for (int i = 0; i < colors.size(); i++)
    {
        _colors[i] = colors[i];
    }

    // Allow light colorizer to not force color
    if (refresh)
    {
        Refresh();
    }
    else
    {
        std::vector<UnityEngine::Color> nonNullColors;
        for (auto& color: colors) {
            if (color)
                nonNullColors.emplace_back(*color);
        }
        nonNullColors.shrink_to_fit();
        SetSOs(nonNullColors);
    }
}

void LightColorizer::Reset() {
    for (int i = 0; i < COLOR_FIELDS; i++)
    {
        GlobalColor[i] = std::nullopt;
    }
}

void LightColorizer::SetSOs(std::vector<UnityEngine::Color> colors) {
    for (int i = 0; i < colors.size(); i++)
    {
        _simpleColorSOs[i]->SetColor(colors[i]);
    }

    LightColorChanged.invoke(_eventType.value, colors);
}

void LightColorizer::Refresh() {
    auto colors = getColor();
    SetSOs(colors);

    _lightSwitchEventEffect->ProcessLightSwitchEvent(_lightSwitchEventEffect->prevLightSwitchBeatmapEventDataValue, true);
}

void LightColorizer::InitializeSO(const std::string &id, int index) {
    auto colorSOAcessor = il2cpp_utils::FindField(classof(LightSwitchEventEffect*), id);
    auto lightMultSO = CRASH_UNLESS(il2cpp_utils::GetFieldValue<GlobalNamespace::MultipliedColorSO>(_lightSwitchEventEffect, colorSOAcessor));

    Color multiplierColor = lightMultSO.multiplierColor;
    auto lightSO = lightMultSO.baseColor;
    _originalColors[index] = lightSO->color;

    SafePtr<MultipliedColorSO> mColorSO(ScriptableObject::CreateInstance<MultipliedColorSO*>());
    mColorSO->multiplierColor = multiplierColor;


    if (!_simpleColorSOs[index])
    {
        SafePtr<SimpleColorSO> sColorSO(ScriptableObject::CreateInstance<SimpleColorSO*>());
        sColorSO->SetColor(lightSO->color);
        _simpleColorSOs.emplace(index, sColorSO);
    }

    SafePtr<SimpleColorSO>& sColorSO = _simpleColorSOs[index];

    mColorSO->baseColor = (SimpleColorSO*) sColorSO;

    il2cpp_utils::SetFieldValue<ColorSO*>(_lightSwitchEventEffect, colorSOAcessor, (MultipliedColorSO*) mColorSO);
}

