#pragma once


#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include "GlobalNamespace/ObstacleController.hpp"
#include "GlobalNamespace/StretchableObstacle.hpp"
#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

// We use custom types here to avoid GC deleting our variables
DECLARE_CLASS_CODEGEN(Chroma, OCColorManager, Il2CppObject,

                      public:
                              inline static int _tintColorID = -1;
                              inline static int _addColorID = -1;
                              inline static std::optional<UnityEngine::Color> _globalColor = std::nullopt;
                              UnityEngine::Color _color_Original;
                              std::optional<UnityEngine::Color> _color;


                              static OCColorManager* GetOCColorManager(GlobalNamespace::ObstacleController* oc);
                              static OCColorManager* CreateOCColorManager(GlobalNamespace::ObstacleController* oc, UnityEngine::Color original);
                              static void SetGlobalObstacleColor(UnityEngine::Color color);
                              static void ResetGlobal();
                              void SetObstacleColor(UnityEngine::Color color);
                              void SetActiveColors() const;

                              DECLARE_METHOD(void, Reset);

                              DECLARE_INSTANCE_FIELD(GlobalNamespace::StretchableObstacle*, _stretchableObstacle);
                              DECLARE_INSTANCE_FIELD(GlobalNamespace::ObstacleController*, _oc);

                              DECLARE_CTOR(ctor, GlobalNamespace::ObstacleController* oc, UnityEngine::Color original);

                              REGISTER_FUNCTION(
                              getLogger().debug("Registering LSEColorManager!");
                              REGISTER_METHOD(ctor);
                              
                              REGISTER_FIELD(_stretchableObstacle);
                              REGISTER_FIELD(_oc);
                              )
)


// TODO: Document properly
namespace Chroma {
    class ObstacleColorizer {
    public:
        static void Reset(GlobalNamespace::ObstacleController* oc);
        static void ResetAllObstacleColors();
        static void SetObstacleColor(GlobalNamespace::ObstacleController* oc, UnityEngine::Color color);
        static void SetAllObstacleColors(UnityEngine::Color color);
        static void SetActiveColors(GlobalNamespace::ObstacleController* oc);
        static void SetAllActiveColors();
        static void ClearOCColorManagers();

        /*
         * OC ColorSO holders
         */
        static void OCStart(GlobalNamespace::ObstacleController* oc, UnityEngine::Color original);

        inline static std::unordered_map<GlobalNamespace::ObstacleController*, OCColorManager*> _ocColorManagers  = {};
    };
}