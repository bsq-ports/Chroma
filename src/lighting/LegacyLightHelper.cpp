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

void LegacyLightHelper::Activate(std::span<GlobalNamespace::BasicBeatmapEventData*> eventData) {
  LegacyColorEvents = LegacyLightHelper::ColorMap(eventData.size());
  debugSpamLog("Got the events, checking for legacy %zu", eventData.size());
  for (auto& d : eventData) {
    // TODO: Should we do this or find the root of the nullptr and fix that instead?
    if (d == nullptr) {
      continue;
    }

    if (!ASSIGNMENT_CHECK(classof(BeatmapEventData*), d->klass)) {
      ChromaLogger::Logger.debug("Beatmap data: {} not what expected", il2cpp_utils::ClassStandardName(d->klass).c_str());
      continue;
    }

    debugSpamLog("Checking d {} {}", d->value, d->value >= RGB_INT_OFFSET ? "true" : "false");
    if (d->value >= RGB_INT_OFFSET) {
      auto& list = LegacyColorEvents.try_emplace(d->basicBeatmapEventType.value__).first->second;
      list.emplace_back(d->time, ColorFromInt(d->value));
    }
  }
}

std::optional<Sombrero::FastColor> LegacyLightHelper::GetLegacyColor(GlobalNamespace::BasicBeatmapEventData* beatmapEventData) {
  if (!ChromaController::GetChromaLegacy()) {
    return std::nullopt;
  }

  auto it = LegacyColorEvents.find(beatmapEventData->basicBeatmapEventType.value__);
  if (it != LegacyColorEvents.end()) {
    auto dictionaryID = it->second;
    std::vector<std::pair<float, Sombrero::FastColor>> colors;

    for (std::pair<float, Sombrero::FastColor>& n : dictionaryID) {
      if (n.first <= beatmapEventData->time) {
        colors.push_back(n);
      }
    }

    if (!colors.empty()) {
      return std::make_optional(colors.back().second);
    }
  }

  return std::nullopt;
}

constexpr Sombrero::FastColor LegacyLightHelper::ColorFromInt(int rgb) {
  rgb -= RGB_INT_OFFSET;
  auto red = static_cast<float>((rgb >> 16) & 0x0ff);
  auto green = static_cast<float>((rgb >> 8) & 0x0ff);
  auto blue = static_cast<float>(rgb & 0x0ff);
  return { red / 255.0F, green / 255.0F, blue / 255.0F, 1 };
}