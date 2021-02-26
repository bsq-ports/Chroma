#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/SaberBurnMarkArea.hpp"
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
        static std::vector<std::optional<UnityEngine::Color>> SaberColorOverride;

        static GlobalNamespace::SaberBurnMarkArea *SaberBurnMarkArea;

        static void SetSaberColor(int saberType, std::optional<UnityEngine::Color> color);

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

            BSMColorManager(GlobalNamespace::Saber *bsm, int saberType);

            [[nodiscard]] GlobalNamespace::Saber* getSaber() const;

        public:
            static std::vector<BSMColorManager *> GetBSMColorManager(int saberType);

            static BSMColorManager *
            CreateBSMColorManager(GlobalNamespace::Saber *bsm, int saberType);

            void SetSaberColor(std::optional<UnityEngine::Color> colorNullable);
        };

    private:
        inline static std::vector<BSMColorManager *> _bsmColorManagers;

    };
}

inline static std::unordered_map<int, custom_types::Helpers::StandardCoroutine*> coroutineSabers;

