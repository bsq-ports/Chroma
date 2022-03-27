#pragma once

#include "Chroma.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/BasicBeatmapEventType.hpp"
#include "GlobalNamespace/SaberBurnMarkArea.hpp"
#include "GlobalNamespace/SaberBurnMarkSparkles.hpp"
#include "GlobalNamespace/SaberTrail.hpp"
#include "GlobalNamespace/SetSaberGlowColor.hpp"
#include "GlobalNamespace/SetSaberFakeGlowColor.hpp"
#include "GlobalNamespace/TubeBloomPrePassLight.hpp"
#include "GlobalNamespace/SaberModelContainer.hpp"
#include "GlobalNamespace/SaberModelController.hpp"
#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include <vector>
#include <string>
#include <optional>

#include "ObjectColorizer.hpp"

#include "custom-types/shared/coroutine.hpp"

namespace Chroma {
    class SaberColorizer : public ObjectColorizer<SaberColorizer> {
    private:
        friend class ObjectColorizer<SaberColorizer>;
        GlobalNamespace::SaberTrail* _saberTrail;
        Sombrero::FastColor _trailTintColor;
        GlobalNamespace::TubeBloomPrePassLight* _saberLight;
        GlobalNamespace::SaberType _saberType;
        Sombrero::FastColor _lastColor;
        GlobalNamespace::SaberModelController* _saberModelController;
    public:
        [[nodiscard]] GlobalNamespace::SaberModelController *getSaberModelController() const;

    private:

        explicit SaberColorizer(GlobalNamespace::Saber* saber, GlobalNamespace::SaberModelController* saberModelController);

        // SiraUtil stuff
        inline static std::unordered_set<GlobalNamespace::SaberModelController*> ColorableModels;

        void ColorColorable(Sombrero::FastColor const& color);

    protected:
        std::optional<Sombrero::FastColor> GlobalColorGetter();

        void Refresh();

    public:
        friend class std::pair<GlobalNamespace::SaberModelController const*, SaberColorizer>;
        friend class std::pair<const GlobalNamespace::SaberModelController *const, Chroma::SaberColorizer>;
        SaberColorizer(SaberColorizer const&) = delete;
        static SaberColorizer& New(GlobalNamespace::Saber* saber);

        inline static UnorderedEventCallback<int, GlobalNamespace::SaberModelController*, Sombrero::FastColor const&> SaberColorChanged;

        inline static std::unordered_map<GlobalNamespace::SaberModelController const*, SaberColorizer> Colorizers;


        inline static std::array<std::optional<Sombrero::FastColor>, 2> GlobalColor = {std::nullopt, std::nullopt};

        static bool IsColorable(GlobalNamespace::SaberModelController* saberModelController);
        static void SetColorable(GlobalNamespace::SaberModelController* saberModelController, bool colorable);

        static void GlobalColorize(GlobalNamespace::SaberType saberType, std::optional<Sombrero::FastColor> const& color);
        static void Reset();

        // extensions
        static std::unordered_set<SaberColorizer*> GetColorizerList(GlobalNamespace::SaberType saberType);

        static void RemoveColorizer(GlobalNamespace::SaberModelController* saberModelController);
        static SaberColorizer& GetColorizer(GlobalNamespace::SaberModelController* saberModelController);

        inline static void ColorizeSaber(GlobalNamespace::SaberModelController* saberModelController, std::optional<Sombrero::FastColor> const& color) {
            GetColorizer(saberModelController).Colorize(color);
        }
    };
}

inline static std::unordered_map<int, custom_types::Helpers::StandardCoroutine*> coroutineSabers;

