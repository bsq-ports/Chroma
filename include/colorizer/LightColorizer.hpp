#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/ILightWithId.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include "GlobalNamespace/LightWithIdManager.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"
#include "sombrero/shared/ColorUtils.hpp"

namespace Chroma {
    class LightColorizer {
    private:
        static const int COLOR_FIELDS = 4;

        GlobalNamespace::LightSwitchEventEffect *_lightSwitchEventEffect;
        GlobalNamespace::BeatmapEventType _eventType;
        std::unordered_map<int, std::optional<Sombrero::FastColor>> _colors;

        std::vector<Sombrero::FastColor> _originalColors;
        std::unordered_map<int, SafePtr<GlobalNamespace::SimpleColorSO>> _simpleColorSOs;

        LightColorizer(GlobalNamespace::LightSwitchEventEffect *lightSwitchEventEffect,GlobalNamespace::BeatmapEventType beatmapEventType);

    public:
        static std::shared_ptr<LightColorizer> New(GlobalNamespace::LightSwitchEventEffect *lightSwitchEventEffect,GlobalNamespace::BeatmapEventType beatmapEventType);

        inline static std::vector<std::optional<Sombrero::FastColor>> GlobalColor {std::nullopt,std::nullopt,std::nullopt,std::nullopt };

        inline static UnorderedEventCallback<GlobalNamespace::BeatmapEventType, std::vector<Sombrero::FastColor>> LightColorChanged;

        inline static std::unordered_map<int, std::shared_ptr<LightColorizer>> Colorizers;

        std::unordered_map<int, GlobalNamespace::ILightWithId *> Lights{};

        std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>> LightsPropagationGrouped{};



        std::vector<Sombrero::FastColor> getColor();

        static void GlobalColorize(bool refresh, std::vector<std::optional<Sombrero::FastColor>> colors);

        static void RegisterLight(UnityEngine::MonoBehaviour *lightWithId, std::optional<int> lightId);

        void Colorize(bool refresh, std::vector<std::optional<Sombrero::FastColor>>& colors);

        static void Reset();

        // extensions
        inline static std::shared_ptr<LightColorizer> GetLightColorizer(GlobalNamespace::BeatmapEventType beatmapEventType) {
            auto it = Colorizers.find(beatmapEventType.value);
            if (it == Colorizers.end()) {
                return nullptr;
            }

            return it->second;
        }

        inline static void ColorizeLight(GlobalNamespace::BeatmapEventType beatmapEventType, bool refresh, std::vector<std::optional<Sombrero::FastColor>> colors) {
            CRASH_UNLESS(GetLightColorizer(beatmapEventType))->Colorize(refresh, colors);
        }

    private:
        void SetSOs(std::vector<Sombrero::FastColor> colors);

        void Refresh();

        void InitializeSO(const std::string &id, int index);
    };
}



