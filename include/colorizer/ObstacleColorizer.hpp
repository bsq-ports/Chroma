#pragma once

#include "main.hpp"
#include "Chroma.hpp"
#include "ObjectColorizer.hpp"

#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include "GlobalNamespace/ObstacleController.hpp"
#include "GlobalNamespace/StretchableObstacle.hpp"
#include "GlobalNamespace/ObstacleControllerBase.hpp"
#include "GlobalNamespace/MaterialPropertyBlockController.hpp"
#include "GlobalNamespace/ParametricBoxFrameController.hpp"
#include "GlobalNamespace/ParametricBoxFakeGlowController.hpp"

#include "UnityEngine/MaterialPropertyBlock.hpp"

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

        GlobalNamespace::ParametricBoxFrameController *_obstacleFrame;
        GlobalNamespace::ParametricBoxFakeGlowController *_obstacleFakeGlow;
        float _addColorMultiplier;
        float _obstacleCoreLerpToWhiteFactor;
        ArrayW<GlobalNamespace::MaterialPropertyBlockController *> _materialPropertyBlockControllers;


        explicit ObstacleColorizer(GlobalNamespace::ObstacleControllerBase *obstacleController);

    protected:
        std::optional<Sombrero::FastColor> GlobalColorGetter() override {
            return GlobalColor;
        }

        void Refresh() override {
            // We do not handle coloring in obstacle colorable
            if (ObstacleColorable) return;

            Sombrero::FastColor const &color = getColor();
            if (color == Sombrero::FastColor(_obstacleFrame->color)) {
                return;
            }

            _obstacleFrame->color = color;
            static auto Refresh = FPtrWrapper<&GlobalNamespace::ParametricBoxFakeGlowController::Refresh>::get();
            _obstacleFrame->Refresh();
            if (_obstacleFakeGlow) {
                _obstacleFakeGlow->color = color;
                Refresh(_obstacleFakeGlow);
            }

            Sombrero::FastColor value = color * _addColorMultiplier;
            value.a = 0.0f;
            static auto ApplyChanges = FPtrWrapper<&GlobalNamespace::MaterialPropertyBlockController::ApplyChanges>::get();
            static auto SetColor = FPtrWrapper<static_cast<void (UnityEngine::MaterialPropertyBlock::*)(int,
                                                                                                        UnityEngine::Color)>(&UnityEngine::MaterialPropertyBlock::SetColor)>::get();

            for (auto &materialPropertyBlockController: _materialPropertyBlockControllers) {
                if(!materialPropertyBlockController->materialPropertyBlock) {
                    continue;
                }
                
                Sombrero::FastColor white = Sombrero::FastColor::white();
                SetColor(materialPropertyBlockController->materialPropertyBlock, _addColorID(), value);
                SetColor(materialPropertyBlockController->materialPropertyBlock, _tintColorID(),
                         Sombrero::FastColor::Lerp(color, white, _obstacleCoreLerpToWhiteFactor));
                ApplyChanges(materialPropertyBlockController);
            }
        }


    public:
        inline static bool ObstacleColorable = false;

        static std::shared_ptr<ObstacleColorizer> New(GlobalNamespace::ObstacleControllerBase *obstacleController);

        inline static std::unordered_map<GlobalNamespace::ObstacleControllerBase *, std::shared_ptr<ObstacleColorizer>> Colorizers;
        inline static std::optional<Sombrero::FastColor> GlobalColor;

        static void GlobalColorize(std::optional<Sombrero::FastColor> const &color);

        static void Reset();

        // extensions
        inline static std::shared_ptr<ObstacleColorizer>
        GetObstacleColorizer(GlobalNamespace::ObstacleControllerBase *obstacleController) {
            auto it = Colorizers.find(obstacleController);
            if (it == Colorizers.end())
                return nullptr;

            return it->second;
        }

        inline static void ColorizeObstacle(GlobalNamespace::ObstacleControllerBase *obstacleControllerBase,
                                            std::optional<Sombrero::FastColor> const &color) {
            CRASH_UNLESS(GetObstacleColorizer(obstacleControllerBase))->Colorize(color);
        }
    };
}