#include "ChromaConfig.hpp"
#include "Chroma.hpp"
#include "hooks/SceneTransition/SceneTransitionHelper.hpp"

#include "lighting/LegacyLightHelper.hpp"
#include "ChromaController.hpp"
#include "lighting/LightIDTableManager.hpp"

#include "GlobalNamespace/BeatmapEnvironmentHelper.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"

#include "utils/ChromaUtils.hpp"

using namespace CustomJSONData;
using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;

void SceneTransitionHelper::Patch(GlobalNamespace::IDifficultyBeatmap* customBeatmapData) {
    if (!customBeatmapData)
        return;

    std::optional<CustomBeatmapData *> customBeatmapDataCustom = il2cpp_utils::try_cast<CustomJSONData::CustomBeatmapData >(customBeatmapData->get_beatmapData());
    if (customBeatmapDataCustom) {
        SceneTransitionHelper::BasicPatch(customBeatmapData, *customBeatmapDataCustom);
    }
}

template<typename RValue = ValueUTF16, typename StringValue = std::u16string_view>
static bool CheckIfInArrayOrKey(RValue const& val, const StringValue stringToCheck) {
    if (val.IsArray()) {
        for (auto &element : val.GetArray()) {
            if (element.IsString() && element.GetString() == stringToCheck)
                return true;
        }
    }

    if (val.IsObject()) {
        for (auto const& element : val.GetObject()) {
            if (element.name.IsString() && element.name.GetString() == stringToCheck)
                return true;
        }
    }

    return false;
}

template<typename RValue = ValueUTF16, typename StringValue = std::u16string_view>
inline static bool CheckIfInArrayOrKey(std::optional<std::reference_wrapper<RValue>> const val, const StringValue stringToCheck) {
    if (!val)
        return false;

    return CheckIfInArrayOrKey<RValue, StringValue>(val->get(), stringToCheck);
}


void SceneTransitionHelper::Patch(GlobalNamespace::IDifficultyBeatmap* customBeatmapData, OverrideEnvironmentSettings*& overrideEnvironmentSettings) {
    if (!customBeatmapData)
        return;

    std::optional<CustomBeatmapData *> customBeatmapDataCustomOpt = il2cpp_utils::try_cast<CustomJSONData::CustomBeatmapData>(customBeatmapData->get_beatmapData());
    if (customBeatmapDataCustomOpt) {
        auto customBeatmapDataCustom = *customBeatmapDataCustomOpt;
        bool chromaRequirement = SceneTransitionHelper::BasicPatch(customBeatmapData, customBeatmapDataCustom);
        if (chromaRequirement && getChromaConfig().environmentEnhancementsEnabled.GetValue()) {

            if (overrideEnvironmentSettings && customBeatmapDataCustom->levelCustomData && CheckIfInArrayOrKey(customBeatmapDataCustom->levelCustomData->value, ENVIRONMENTREMOVAL)) {
                overrideEnvironmentSettings = nullptr;
            }
            if (overrideEnvironmentSettings && customBeatmapDataCustom->customData && CheckIfInArrayOrKey<rapidjson::Value>(customBeatmapDataCustom->customData->value, ENVIRONMENT)) {
                overrideEnvironmentSettings = nullptr;
            }

            if (!overrideEnvironmentSettings) {
                getLogger().debug("Environment removal!");
            }
        }
    }
}



bool SceneTransitionHelper::BasicPatch(GlobalNamespace::IDifficultyBeatmap* customBeatmapDifficultyData, CustomJSONData::CustomBeatmapData* customBeatmapDataCustom) {
    ChromaController::TutorialMode = false;
    auto environmentInfo = BeatmapEnvironmentHelper::GetEnvironmentInfo(customBeatmapDifficultyData);

    LightIDTableManager::SetEnvironment(to_utf8(csstrtostr(environmentInfo->serializedName)));

    bool chromaRequirement = false;

    ChromaController::infoDatCopy = std::nullopt;

    if (customBeatmapDataCustom->levelCustomData) {
        auto dynData = customBeatmapDataCustom->levelCustomData->value;

        getLogger().debug("Level custom data value: %s", dynData ? "true" : "false");

        if (dynData) {
            ValueUTF16 &rapidjsonData = *dynData;

            // Copy this since it gets freed later on.
            DocumentUTF16 doc;
            doc.CopyFrom(rapidjsonData, doc.GetAllocator());

            ChromaController::infoDatCopy = std::make_optional(std::move(doc));

            auto requirements = rapidjsonData.FindMember(u"_requirements");

            if (requirements != rapidjsonData.MemberEnd()) {
                chromaRequirement |= CheckIfInArrayOrKey(requirements->value, REQUIREMENTNAME);
            }

            auto suggestions = rapidjsonData.FindMember(u"_suggestions");

            if (suggestions != rapidjsonData.MemberEnd()) {
                chromaRequirement |= CheckIfInArrayOrKey(suggestions->value, REQUIREMENTNAME);
            }

        }
    }

    // please let me remove this shit
    bool legacyOverride = false;


    auto beatmapEvents = customBeatmapDifficultyData->get_beatmapData()->beatmapEventsData->items;
    auto length = beatmapEvents->Length();
    getLogger().debug("Checking at most %d for ChromaLite notes", (int) length);
    for (int i = 0; i < length; i++) {
        auto event = beatmapEvents->get(i);

        if (!event) continue;

        if (event->value >= LegacyLightHelper::RGB_INT_OFFSET)
            legacyOverride = true;

        if (legacyOverride)
            break;
    }

    if (legacyOverride) {
        getLogger().warning("Legacy Chroma Detected...");
        getLogger().warning(
                "Please do not use Legacy Chroma for new maps as it is deprecated and its functionality in future versions of Chroma cannot be guaranteed");
    }

    ChromaController::SetChromaLegacy(legacyOverride);
    ChromaController::setChromaRequired(chromaRequirement);


    return ChromaController::ChromaRequired();
}