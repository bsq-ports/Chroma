#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/SaberBurnMarkArea.hpp"
#include "GlobalNamespace/SaberBurnMarkSparkles.hpp"
#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include <vector>
#include <string>
#include <optional>

#include "custom-types/shared/coroutine.hpp"


// TODO: Document properly
// TODO: Does this need to become a custom type?
namespace Chroma {
    class SaberColorizer {

    public:
        inline static std::vector<std::function<void()>> saberCallbacks;
        static std::vector<std::optional<UnityEngine::Color>> SaberColorOverride;

        static void registerCallback(std::function<void()> callback);

        static void clearCallbacks();

        static void SetSaberColor(int saberType, UnityEngine::Color color);

        static void
        SetAllSaberColors(std::optional<UnityEngine::Color> color0, std::optional<UnityEngine::Color> color1);

        static void ClearBSMColorManagers();

        /*
         * BSM ColorSO holders
         */

        static void BSMStart(GlobalNamespace::Saber *bcm, int saberType);

        class BSMColorManager {
        private:
            int _saberType;
            UnityEngine::Color _lastColor;


            inline static GlobalNamespace::SaberBurnMarkArea *saberBurnMarkArea = nullptr;
            inline static GlobalNamespace::SaberBurnMarkSparkles* saberBurnMarkSparkles = nullptr;

            [[nodiscard]] GlobalNamespace::Saber* getSaber() const;

        public:
            BSMColorManager(GlobalNamespace::Saber *bsm, int saberType);

            static std::shared_ptr<BSMColorManager> GetBSMColorManager(int saberType);

            static std::shared_ptr<BSMColorManager> CreateBSMColorManager(GlobalNamespace::Saber *bsm, int saberType);

            void SetSaberColor(UnityEngine::Color color);
        };

    private:
        inline static std::unordered_map<int, std::shared_ptr<BSMColorManager>> _bsmColorManagers;

    };
}

inline static std::unordered_map<int, custom_types::Helpers::StandardCoroutine*> coroutineSabers;

