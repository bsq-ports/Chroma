#pragma once

#include "main.hpp"
#include "Chroma.hpp"
#include "lighting/ChromaLightSwitchEventEffect.hpp"

#include "sombrero/shared/ColorUtils.hpp"

#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "custom-json-data/shared/VList.h"

#include "GlobalNamespace/ILightWithId.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "GlobalNamespace/BasicBeatmapEventType.hpp"
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


        int lightId;
        std::unordered_map<int, std::optional<Sombrero::FastColor>> _colors;

        std::array<Sombrero::FastColor, 4> _originalColors;
        std::unordered_map<int, SafePtrUnity<GlobalNamespace::SimpleColorSO>> _simpleColorSOs;

        LightColorizer(ChromaLightSwitchEventEffect *lightSwitchEventEffect,
                       GlobalNamespace::LightWithIdManager *lightManager);


    inline static std::vector<std::tuple<int, std::function<void(LightColorizer&)>>> _contracts;
    inline static std::vector<std::tuple<int, std::function<void(LightColorizer&)>>> _contractsByLightID;


    public:
        ChromaLightSwitchEventEffect *_lightSwitchEventEffect;

        using LightColorOptionalPalette = std::array<std::optional<Sombrero::FastColor>, 4>;
        using LightColorPalette = std::array<Sombrero::FastColor, 4>;

        friend class std::pair<const int, Chroma::LightColorizer>;
        friend class std::pair<int, LightColorizer>;
        friend class std::pair<const int, Chroma::LightColorizer>;
        static LightColorizer &New(ChromaLightSwitchEventEffect *lightSwitchEventEffect,
                                   GlobalNamespace::LightWithIdManager *lightManager);

        inline static LightColorOptionalPalette GlobalColor{std::nullopt, std::nullopt,
                                                            std::nullopt, std::nullopt};

        inline static UnorderedEventCallback<GlobalNamespace::BasicBeatmapEventType, std::array<Sombrero::FastColor, 4>> LightColorChanged;

        inline static std::unordered_map<int, LightColorizer> Colorizers;
        inline static std::unordered_map<int, LightColorizer*> ColorizersByLightID;

        VList<GlobalNamespace::ILightWithId *> Lights{};

        std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>> LightsPropagationGrouped{};


        [[nodiscard]] LightColorPalette getColor() const {
            LightColorPalette colors;
            for (int i = 0; i < COLOR_FIELDS; i++) {
                std::optional<Sombrero::FastColor> color;

                auto colorIt = _colors.find(i);
                if (colorIt != _colors.end()) {
                    color = colorIt->second;
                }

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

        // dont use this please
        // cant be fucked to make an overload for this
        std::vector<GlobalNamespace::ILightWithId*> GetPropagationLightWithIds(std::vector<int> const& ids);
        [[nodiscard]] std::vector<GlobalNamespace::ILightWithId*> GetLightWithIds(std::vector<int> const &ids) const;

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

            SetSOs(getColor());

            // Allow light colorizer to not force color
            if (!refresh) return;

            Refresh(selectLights);
        }

        static void CreateLightColorizerContractByLightID(int lightId, std::function<void(LightColorizer&)> callback);

        static void CreateLightColorizerContract(GlobalNamespace::BasicBeatmapEventType type, std::function<void(LightColorizer&)> callback);

        static void CompleteContracts(ChromaLightSwitchEventEffect* chromaLightSwitchEventEffect);

        static void Reset();

        // extensions
        inline static LightColorizer* GetLightColorizer(GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType) {
            auto it = Colorizers.find(BasicBeatmapEventType.value);
            if (it == Colorizers.end()) {
                return nullptr;
            }

            return &it->second;
        }

        inline static LightColorizer* GetLightColorizerLightID(int lightId) {
            auto it = ColorizersByLightID.find(lightId);
            if (it == ColorizersByLightID.end()) {
                return nullptr;
            }

            return it->second;
        }

        inline static void ColorizeLight(GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType, bool refresh,
                                         LightColorOptionalPalette const &colors) {
            auto colorizer = GetLightColorizer(BasicBeatmapEventType);
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
            return;
            static auto SetColor = FPtrWrapper<&GlobalNamespace::SimpleColorSO::SetColor>::get();

            for (int i = 0; i < colors.size(); i++) {
                SetColor((GlobalNamespace::SimpleColorSO *) _simpleColorSOs[i], colors[i]);
            }

            LightColorChanged.invoke(lightId, colors);
        }

        void Refresh(std::optional<std::vector<GlobalNamespace::ILightWithId*>> const& selectLights) const {
            _lightSwitchEventEffect->Refresh(false, selectLights);
        }

    };
}



