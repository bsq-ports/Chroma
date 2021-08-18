#pragma once

#include "Chroma.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
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
    class SaberColorizer : public ObjectColorizer {
    private:
        GlobalNamespace::SaberTrail* _saberTrail;
        Sombrero::FastColor _trailTintColor;
        GlobalNamespace::TubeBloomPrePassLight* _saberLight;
        GlobalNamespace::SaberType _saberType;
        Sombrero::FastColor _lastColor;
        GlobalNamespace::SaberModelController* _saberModelController;

        explicit SaberColorizer(GlobalNamespace::Saber* saber);

        // SiraUtil stuff
        inline static std::unordered_set<GlobalNamespace::SaberModelController*> ColorableModels;

        void ColorColorable(Sombrero::FastColor color);

    protected:
        std::optional<Sombrero::FastColor> GlobalColorGetter() override;

        void Refresh() override;

    public:
        static std::shared_ptr<SaberColorizer> New(GlobalNamespace::Saber* saber);

        inline static UnorderedEventCallback<int, GlobalNamespace::SaberModelController*, Sombrero::FastColor> SaberColorChanged;

        inline static std::unordered_map<GlobalNamespace::SaberModelController*, std::shared_ptr<SaberColorizer>> Colorizers;


        inline static std::vector<std::optional<Sombrero::FastColor>> GlobalColor = {std::nullopt, std::nullopt};

        static bool IsColorable(GlobalNamespace::SaberModelController* saberModelController);
        static void SetColorable(GlobalNamespace::SaberModelController* saberModelController, bool colorable);

        static void GlobalColorize(GlobalNamespace::SaberType saberType, std::optional<Sombrero::FastColor> const& color);
        static void Reset();

        // extensions
        static std::unordered_set<SaberColorizer*> GetColorizerList(GlobalNamespace::SaberType saberType);

        static void RemoveColorizer(GlobalNamespace::SaberModelController* saberModelController);
        static std::shared_ptr<SaberColorizer>& GetColorizer(GlobalNamespace::SaberModelController* saberModelController);

        inline static void ColorizeSaber(GlobalNamespace::SaberModelController* saberModelController, std::optional<Sombrero::FastColor> const& color) {
            GetColorizer(saberModelController)->Colorize(color);
        }
    };
}

inline static std::unordered_map<int, custom_types::Helpers::StandardCoroutine*> coroutineSabers;

