#include "lighting/LegacyLightHelper.hpp"
#include "main.hpp"
#include "Chroma.hpp"
#include "utils/ChromaUtils.hpp"

#include "ChromaController.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;

LegacyLightHelper::ColorMap LegacyLightHelper::LegacyColorEvents = LegacyLightHelper::ColorMap();

void LegacyLightHelper::Activate(const std::vector<GlobalNamespace::BeatmapEventData*>& eventData) {
    static auto contextLogger = getLogger().WithContext(ChromaLogger::LegacyLightColor);

    LegacyColorEvents = LegacyLightHelper::ColorMap();
    debugSpamLog(contextLogger, "Got the events, checking for legacy %d", eventData.size());
    for (auto& d : eventData)
    {
        // TODO: Should we do this or find the root of the nullptr and fix that instead?
        if (d == nullptr) {
            continue;
        }

        if (!ASSIGNMENT_CHECK(classof(BeatmapEventData*), d->klass)) {
            getLogger().debug("Beatmap data: %s not what expected", il2cpp_utils::ClassStandardName(d->klass).c_str());
            continue;
        }


        debugSpamLog(contextLogger, "Checking d %d %s", d->value, d->value >= RGB_INT_OFFSET ? "true" : "false");
        if (d->value >= RGB_INT_OFFSET)
        {
            auto it = LegacyColorEvents.find(d->type);
            auto list = it != LegacyColorEvents.end() ? it->second : std::vector<pair<float, Sombrero::FastColor>>();

            list.emplace_back(d->time, ColorFromInt(d->value));
            LegacyColorEvents[d->type] = list;
        }
    }
}

std::optional<Sombrero::FastColor> LegacyLightHelper::GetLegacyColor(GlobalNamespace::BeatmapEventData *beatmapEventData) {
    if (!ChromaController::GetChromaLegacy())
        return std::nullopt;

    auto it = LegacyColorEvents.find(beatmapEventData->type);
    if (it != LegacyColorEvents.end()) {
        auto dictionaryID = it->second;
        std::vector<pair<float, Sombrero::FastColor>> colors;

        for (pair<float, Sombrero::FastColor>& n : dictionaryID) {
            if (n.first <= beatmapEventData->time)
                colors.push_back(n);
        }


        if (!colors.empty())
        {
            return std::make_optional(colors.back().second);
        }
    }

    return std::nullopt;
}

Sombrero::FastColor LegacyLightHelper::ColorFromInt(int rgb) {
    rgb -= RGB_INT_OFFSET;
    auto red = (float) ((rgb >> 16) & 0x0ff);
    auto green = (float) ((rgb >> 8) & 0x0ff);
    auto blue = (float ) (rgb & 0x0ff);
    return {red / 255.0f, green / 255.0f, blue / 255.0f, 1};
}