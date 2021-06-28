#pragma once

#include "utils/EventCallback.hpp"

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

// TODO: Document properly
namespace Chroma {
    class LSEColorManager;

    class LightColorizer {
    private:
        static const int COLOR_FIELDS = 4;

        GlobalNamespace::LightSwitchEventEffect *_lightSwitchEventEffect;
        GlobalNamespace::BeatmapEventType _eventType;
        std::vector<std::optional<UnityEngine::Color>> _colors;

        std::vector<UnityEngine::Color> _originalColors;
        std::unordered_map<int, SafePtr<GlobalNamespace::SimpleColorSO>> _simpleColorSOs;

        LightColorizer(GlobalNamespace::LightSwitchEventEffect *lightSwitchEventEffect,GlobalNamespace::BeatmapEventType beatmapEventType);

    public:
        static std::shared_ptr<LightColorizer> New(GlobalNamespace::LightSwitchEventEffect *lightSwitchEventEffect,GlobalNamespace::BeatmapEventType beatmapEventType);

        inline static std::vector<std::optional<UnityEngine::Color>> GlobalColor {std::nullopt,std::nullopt,std::nullopt,std::nullopt };

        inline static UnorderedEventCallback<GlobalNamespace::BeatmapEventType, std::vector<UnityEngine::Color>> LightColorChanged;

        inline static std::unordered_map<int, std::shared_ptr<LightColorizer>> Colorizers;

        std::unordered_map<int, GlobalNamespace::ILightWithId *> Lights;

        std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>> LightsPropagationGrouped;



        std::vector<UnityEngine::Color> getColor() const;

        static void GlobalColorize(bool refresh, std::vector<std::optional<UnityEngine::Color>> colors);

        static void RegisterLight(UnityEngine::MonoBehaviour *lightWithId, std::optional<int> lightId);

        void Colorize(bool refresh, std::vector<std::optional<UnityEngine::Color>> colors);

        static void Reset();

        // extensions
        inline static std::shared_ptr<LightColorizer> GetLightColorizer(GlobalNamespace::BeatmapEventType beatmapEventType) {
            if (Colorizers.find(beatmapEventType) == Colorizers.end())
                return nullptr;

            return Colorizers[beatmapEventType];
        }

        inline static void ColorizeLight(GlobalNamespace::BeatmapEventType beatmapEventType, bool refresh, std::vector<std::optional<UnityEngine::Color>> colors) {
            GetLightColorizer(beatmapEventType)->Colorize(refresh, colors);
        }

    private:
        void SetSOs(std::vector<UnityEngine::Color> colors);

        void Refresh();

        void InitializeSO(const std::string &id, int index);
    };
}

typedef std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>> LightGroupMap;



