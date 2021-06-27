#pragma once

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
#include "utils/EventCallback.hpp"

#include "custom-types/shared/coroutine.hpp"

namespace Chroma {
    class SaberColorizer : public ObjectColorizer {
    private:
        GlobalNamespace::SaberTrail* _saberTrail;
        UnityEngine::Color _trailTintColor;
        std::vector<GlobalNamespace::SetSaberGlowColor*> _setSaberGlowColors;
        std::vector<GlobalNamespace::SetSaberFakeGlowColor*> _setSaberFakeGlowColors;
        GlobalNamespace::TubeBloomPrePassLight* _saberLight;
        GlobalNamespace::SaberType _saberType;
        bool _doColor;
        UnityEngine::Color _lastColor;
        GlobalNamespace::SaberModelController* _saberModelController;

        explicit SaberColorizer(GlobalNamespace::Saber* saber);

        static std::unordered_set<std::shared_ptr<SaberColorizer>> GetOrCreateColorizerList(GlobalNamespace::SaberType saberType);

        // SiraUtil stuff
        bool IsColorable(GlobalNamespace::SaberModelController* saberModelController);

        void ColorColorable(UnityEngine::Color color);



    public:
        static std::shared_ptr<SaberColorizer> New(GlobalNamespace::Saber* saber);

    protected:
        std::optional<UnityEngine::Color> GlobalColorGetter() override;

        void Refresh() override;

    public:
        inline static UnorderedEventCallback<int, UnityEngine::Color> SaberColorChanged;
        inline static std::unordered_map<int, std::unordered_set<std::shared_ptr<SaberColorizer>>> Colorizers;
        inline static std::vector<std::optional<UnityEngine::Color>> GlobalColor = {std::nullopt, std::nullopt};


        static void GlobalColorize(std::optional<UnityEngine::Color> color, GlobalNamespace::SaberType saberType);
        static void Reset();

        inline static std::unordered_set<std::shared_ptr<SaberColorizer>> GetSaberColorizer(GlobalNamespace::SaberType saberType) {
            return GetSaberColorizer(saberType.value);
        }

        // extensions
        inline static std::unordered_set<std::shared_ptr<SaberColorizer>> GetSaberColorizer(int saberType) {
            if (Colorizers.find(saberType) == Colorizers.end())
                return {};

            return Colorizers[saberType];
        }

        inline static void ColorizeSaber(GlobalNamespace::SaberType saberType, std::optional<UnityEngine::Color> color) {
            for(auto& colorizer : GetSaberColorizer(saberType)) {
                colorizer->Colorize(color);
            }
        }
    };
}

inline static std::unordered_map<int, custom_types::Helpers::StandardCoroutine*> coroutineSabers;

