#pragma once

#include "main.hpp"
#include "Chroma.hpp"
#include "lighting/ChromaLightSwitchEventEffect.hpp"

#include "sombrero/shared/ColorUtils.hpp"

#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

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

namespace Chroma {
    class LightColorizer {
    private:
        static const int COLOR_FIELDS = 4;

        ChromaLightSwitchEventEffect *_lightSwitchEventEffect;
        GlobalNamespace::BeatmapEventType _eventType;
        std::unordered_map<int, std::optional<Sombrero::FastColor>> _colors;

        std::array<Sombrero::FastColor, 4> _originalColors;
        std::unordered_map<int, SafePtr<GlobalNamespace::SimpleColorSO>> _simpleColorSOs;

        LightColorizer(ChromaLightSwitchEventEffect *lightSwitchEventEffect,
                       GlobalNamespace::BeatmapEventType beatmapEventType,
                       GlobalNamespace::LightWithIdManager* lightManager);

    public:
        using LightColorOptionalPalette = std::array<std::optional<Sombrero::FastColor>, 4>;
        using LightColorPalette = std::array<Sombrero::FastColor, 4>;

        static std::shared_ptr<LightColorizer> New(ChromaLightSwitchEventEffect *lightSwitchEventEffect,
                                                   GlobalNamespace::BeatmapEventType beatmapEventType,
                                                   GlobalNamespace::LightWithIdManager* lightManager);

        inline static LightColorOptionalPalette GlobalColor{std::nullopt, std::nullopt,
                                                            std::nullopt, std::nullopt};

        inline static UnorderedEventCallback<GlobalNamespace::BeatmapEventType, std::array<Sombrero::FastColor, 4>> LightColorChanged;

        inline static std::unordered_map<int, std::shared_ptr<LightColorizer>> Colorizers;

        std::vector<GlobalNamespace::ILightWithId *> Lights{};

        std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>> LightsPropagationGrouped{};


        LightColorPalette getColor() {
            LightColorPalette colors;
            for (int i = 0; i < COLOR_FIELDS; i++) {
                auto color = _colors[i];

                if (!color)
                    color = GlobalColor[i];

                if (!color)
                    color = _originalColors[i];


                colors[i] = *color;
            }

            return colors;
        }

        inline static void GlobalColorize(std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& selectLights, LightColorOptionalPalette const &colors) {
            GlobalColorize(true, selectLights, colors);
        }
        inline static void GlobalColorize(bool refresh, LightColorOptionalPalette const &colors) {
            GlobalColorize(refresh, std::nullopt, colors);
        }
        static void GlobalColorize(bool refresh, std::optional<std::vector<GlobalNamespace::ILightWithId*>> const&  lights, LightColorOptionalPalette const &colors);

        static void RegisterLight(UnityEngine::MonoBehaviour *lightWithId, std::optional<int> lightId);

        // dont use this please
        // cant be fucked to make an overload for this
        std::vector<GlobalNamespace::ILightWithId*> GetPropagationLightWithIds(std::vector<int> const& ids);
        std::vector<GlobalNamespace::ILightWithId*> GetLightWithIds(std::vector<int> const& ids);

        inline void Colorize(std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& selectLights, LightColorOptionalPalette const& colors) {
            Colorize(true, selectLights, colors);
        }

        inline void Colorize(bool refresh, LightColorOptionalPalette const& colors) {
            Colorize(refresh, std::nullopt, colors);
        }

        void Colorize(bool refresh, std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& selectLights, LightColorOptionalPalette const &colors) {
            for (int i = 0; i < colors.size(); i++) {
                _colors[i] = colors[i];
            }

            // Allow light colorizer to not force color
            if (refresh) {
                Refresh(selectLights);
            } else {
                SetSOs(getColor());
            }
        }

        static void Reset();

        // extensions
        inline static std::shared_ptr<LightColorizer>
        GetLightColorizer(GlobalNamespace::BeatmapEventType beatmapEventType) {
            auto it = Colorizers.find(beatmapEventType.value);
            if (it == Colorizers.end()) {
                return nullptr;
            }

            return it->second;
        }

        inline static void ColorizeLight(GlobalNamespace::BeatmapEventType beatmapEventType, bool refresh,
                                         LightColorOptionalPalette const &colors) {
            auto colorizer = GetLightColorizer(beatmapEventType);
            CRASH_UNLESS(colorizer)->Colorize(refresh, colors);
        }

        void InitializeSO(
                GlobalNamespace::ColorSO*& lightColor0,
                GlobalNamespace::ColorSO*& highlightColor0,
                GlobalNamespace::ColorSO*& lightColor1,
                GlobalNamespace::ColorSO*& highlightColor1,
                GlobalNamespace::ColorSO*& lightColor0Boost,
                GlobalNamespace::ColorSO*& highlightColor0Boost,
                GlobalNamespace::ColorSO*& lightColor1Boost,
                GlobalNamespace::ColorSO*& highlightColor1Boost
        );

    private:

        void SetSOs(LightColorPalette const &colors) {
            static auto SetColor = FPtrWrapper<&GlobalNamespace::SimpleColorSO::SetColor>::get();

            for (int i = 0; i < colors.size(); i++) {
                SetColor((GlobalNamespace::SimpleColorSO *) _simpleColorSOs[i], colors[i]);
            }

            LightColorChanged.invoke(_eventType.value, colors);
        }

        void Refresh(std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& selectLights) {
            _lightSwitchEventEffect->Refresh(false, selectLights);
        }

    };
}



