#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include "GlobalNamespace/SaberBurnMarkArea.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/Saber.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include <vector>
#include <string>
#include <optional>

// TODO: Document properly



DECLARE_CLASS_INTERFACES(Chroma, ChangeColorCoroutine, "System", "Object", sizeof(Il2CppObject),
                         il2cpp_utils::GetClassFromName("System.Collections", "IEnumerator"),

                         DECLARE_CTOR(ctor, GlobalNamespace::Saber *instance, UnityEngine::Color color);

                                 DECLARE_INSTANCE_FIELD(Il2CppObject*, current);
                                 DECLARE_INSTANCE_FIELD(bool, hasWaited);

                                 DECLARE_INSTANCE_FIELD(GlobalNamespace::Saber*, instance);
                                 DECLARE_INSTANCE_FIELD(UnityEngine::Color, color);

                                 DECLARE_OVERRIDE_METHOD(bool, MoveNext, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "MoveNext"));
                                 DECLARE_OVERRIDE_METHOD(Il2CppObject*, get_Current, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "get_Current"));
                                 DECLARE_OVERRIDE_METHOD(void, Reset, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "Reset"));

                                 REGISTER_FUNCTION(ChangeColorEnumerator,
                                 getLogger().debug("Registering ChangeColorEnumerator!");

                                 REGISTER_FIELD(current);
                                 REGISTER_FIELD(hasWaited);

                                 REGISTER_FIELD(instance);
                                 REGISTER_FIELD(color);

                                 REGISTER_METHOD(ctor);

                                 REGISTER_METHOD(MoveNext);
                                 REGISTER_METHOD(get_Current);
                                 REGISTER_METHOD(Reset);
                         )
)



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
            GlobalNamespace::Saber *_bsm;
            int _saberType;

            BSMColorManager(GlobalNamespace::Saber *bsm, int saberType);

        public:
            static std::vector<BSMColorManager *> GetBSMColorManager(int saberType);

            static BSMColorManager *
            CreateBSMColorManager(GlobalNamespace::Saber *bsm, int saberType);

            void SetSaberColor(std::optional<UnityEngine::Color> colorNullable);
        };

    private:
        static std::vector<BSMColorManager *> _bsmColorManagers;

    };
}

inline static std::unordered_map<GlobalNamespace::Saber*, Chroma::ChangeColorCoroutine*> coroutineSabers;

