#include "LegacyLightHelper.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;

void LegacyLightHelper::Activate(const std::vector<GlobalNamespace::BeatmapEventData *>& eventData) {
    LegacyColorEvents.clear();
    for (auto& d : eventData)
    {
        // TODO: Should we do this or find the root of the nullptr and fix that instead?
        if (d == nullptr)
            continue;

        if (d->value >= RGB_INT_OFFSET)
        {

            auto list = std::vector<pair<float, UnityEngine::Color>>();
            if (!LegacyColorEvents.contains(d->type)) {
                LegacyColorEvents[d->type] = list;
            } else
                list = LegacyColorEvents[d->type];


            list.emplace_back(d->time, ColorFromInt(d->value));
        }
    }
}

std::optional<UnityEngine::Color> LegacyLightHelper::GetLegacyColor(
        GlobalNamespace::BeatmapEventData *beatmapEventData) {
    auto list = std::vector<pair<float, UnityEngine::Color>>();
    if (LegacyColorEvents.contains(beatmapEventData->type)) {
        auto dictionaryID = LegacyColorEvents[beatmapEventData->type];
        std::vector<pair<float, UnityEngine::Color>> colors;

        for (pair<float, UnityEngine::Color>& n : dictionaryID) {
            if (n.first <= beatmapEventData->time)
                colors.push_back(n);
        }


        if (!colors.empty())
        {
            return std::make_optional(colors.end()->second);
        }
    }

    return std::nullopt;
}

UnityEngine::Color LegacyLightHelper::ColorFromInt(int rgb) {
    rgb -= RGB_INT_OFFSET;
    auto red = (float) ((rgb >> 16) & 0x0ff);
    auto green = (float) ((rgb >> 8) & 0x0ff);
    auto blue = (float ) (rgb & 0x0ff);
    return UnityEngine::Color(red / 255.0f, green / 255.0f, blue / 255.0f);
}