#pragma once


#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include "GlobalNamespace/ObstacleController.hpp"
#include "GlobalNamespace/StretchableObstacle.hpp"
#include <vector>
#include <string>
#include <optional>



// TODO: Document properly
// TODO: Does this need to become a custom type?
namespace Chroma {
    class ObstacleColorizer {
    public:
        static void Reset(GlobalNamespace::ObstacleController* oc);
        static void ResetAllObstacleColors();
        static void SetObstacleColor(GlobalNamespace::ObstacleController* oc, std::optional<UnityEngine::Color> color);
        static void SetAllObstacleColors(std::optional<UnityEngine::Color> color);
        static void SetActiveColors(GlobalNamespace::ObstacleController* oc);
        static void SetAllActiveColors();
        static void ClearOCColorManagers();

        /*
         * OC ColorSO holders
         */
        static void OCStart(GlobalNamespace::ObstacleController* oc);

        class OCColorManager
        {
        private:
            inline static int _tintColorID = -1;
            inline static int _addColorID = -1;
            static std::optional<UnityEngine::Color> _globalColor;
            GlobalNamespace::ObstacleController* _oc;
            UnityEngine::Color _color_Original;
            // TODO: THIS MAY GET GC'ED. WE MAY NEED CUSTOM TYPES
            GlobalNamespace::SimpleColorSO* _color;
            GlobalNamespace::StretchableObstacle* _stretchableObstacle;
            explicit OCColorManager(GlobalNamespace::ObstacleController* oc);

        public:
            static OCColorManager* GetOCColorManager(GlobalNamespace::ObstacleController* oc);
            static OCColorManager* CreateOCColorManager(GlobalNamespace::ObstacleController* oc);
            static void SetGlobalObstacleColor(std::optional<UnityEngine::Color> color);
            static void ResetGlobal();
            void Reset();
            void SetObstacleColor(std::optional<UnityEngine::Color> color);
            void SetActiveColors();
        };
    private:
        inline static std::vector<OCColorManager*> _ocColorManagers  = {};
    };
}

