#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "UnityEngine/Material.hpp"
#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

// We use custom types here to avoid GC deleting our variables
DECLARE_CLASS_CODEGEN(Chroma, BNCColorManager, Il2CppObject,

                      public:

                              static std::optional<UnityEngine::Color> _globalColor;
                              UnityEngine::Color _color_Original;


                              static BNCColorManager* GetBNCColorManager(GlobalNamespace::BombNoteController* nc);

                              static BNCColorManager* CreateBNCColorManager(GlobalNamespace::BombNoteController* nc);

                              static void SetGlobalBombColor(std::optional<UnityEngine::Color> color);

                              static void ResetGlobal();

                              void SetBombColor(std::optional<UnityEngine::Color> color) const;

                              DECLARE_METHOD(void, Reset);
                              DECLARE_INSTANCE_FIELD(UnityEngine::Material*, _bombMaterial);
                              DECLARE_INSTANCE_FIELD(GlobalNamespace::BombNoteController*, _nc);

                              DECLARE_CTOR(ctor, GlobalNamespace::BombNoteController* nc);

                              REGISTER_FUNCTION(Chroma::LSEColorManager,
                              getLogger().debug("Registering LSEColorManager!");
                                  REGISTER_METHOD(ctor);
                                  REGISTER_METHOD(Reset);

                                  REGISTER_FIELD(_bombMaterial);
                                  REGISTER_FIELD(_nc);
                      )
);

// TODO: Document properly
// TODO: Does this need to become a custom type?
namespace Chroma {
    class BombColorizer {
    public:
        static void Reset(GlobalNamespace::BombNoteController* bnc);
        static void ResetAllBombColors();
        static void SetBombColor(GlobalNamespace::BombNoteController* bnc, std::optional<UnityEngine::Color> color);
        static void SetAllBombColors(std::optional<UnityEngine::Color> color);
        static void ClearBNCColorManagers();

        /*
         * NC ColorSO holders
         */
        static void BNCStart(GlobalNamespace::BombNoteController* bnc);
        inline static std::vector<BNCColorManager*> _bncColorManagers = {};
    };
}


