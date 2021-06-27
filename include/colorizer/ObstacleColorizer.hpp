#pragma once

#include "main.hpp"
#include "ObjectColorizer.hpp"

#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include "GlobalNamespace/ObstacleController.hpp"
#include "GlobalNamespace/StretchableObstacle.hpp"
#include "GlobalNamespace/ObstacleControllerBase.hpp"

#include <vector>
#include <string>
#include <optional>
#include <unordered_map>

#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"


namespace Chroma {
    class ObstacleColorizer : public ObjectColorizer {
    private:
        static int _tintColorID();
        static int _addColorID();

     GlobalNamespace::ParametricBoxFrameController* _obstacleFrame;
     GlobalNamespace::ParametricBoxFakeGlowController* _obstacleFakeGlow;
     float _addColorMultiplier;
     float _obstacleCoreLerpToWhiteFactor;
     std::vector<GlobalNamespace::MaterialPropertyBlockController*> _materialPropertyBlockControllers;


    explicit ObstacleColorizer(GlobalNamespace::ObstacleControllerBase* obstacleController);

    protected:
        std::optional<UnityEngine::Color> GlobalColorGetter() override;
        void Refresh() override;


    public:
        static std::shared_ptr<ObstacleColorizer> New(GlobalNamespace::ObstacleControllerBase* obstacleController);
        inline static std::unordered_map<GlobalNamespace::ObstacleControllerBase*, std::shared_ptr<ObstacleColorizer>> Colorizers;
        inline static std::optional<UnityEngine::Color> GlobalColor;
        static void GlobalColorize(std::optional<UnityEngine::Color> color);

        static void Reset();

        // extensions
        inline static std::shared_ptr<ObstacleColorizer> GetObstacleColorizer(GlobalNamespace::ObstacleControllerBase* obstacleController) {
            if (Colorizers.find(obstacleController) == Colorizers.end())
                return nullptr;

            return Colorizers[obstacleController];
        }

        inline static void ColorizeObstacle(GlobalNamespace::ObstacleControllerBase* obstacleControllerBase, std::optional<UnityEngine::Color> color) {
            GetObstacleColorizer(obstacleControllerBase)->Colorize(color);
        }
    };
}