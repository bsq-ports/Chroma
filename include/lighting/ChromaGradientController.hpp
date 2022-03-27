#pragma once
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>

#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Color.hpp"

#include "GlobalNamespace/BasicBeatmapEventType.hpp"

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "ChromaEventData.hpp"
#include "ChromaController.hpp"

#include "tracks/shared/Animation/Easings.h"
#include "tracks/shared/StaticHolders.hpp"
#include "main.hpp"

namespace Chroma {
    struct ChromaGradientEvent;
}

typedef std::unordered_map<int, Chroma::ChromaGradientEvent> gradientMap;

namespace Chroma {
    struct ChromaGradientEvent {
        Sombrero::FastColor _initcolor;
        Sombrero::FastColor _endcolor;
        float _start;
        float _duration;
        GlobalNamespace::BasicBeatmapEventType _event;
        Functions _easing;

        ChromaGradientEvent(Sombrero::FastColor const& initcolor, Sombrero::FastColor const& endcolor, float start,
                                      float duration, GlobalNamespace::BasicBeatmapEventType eventType,
                                      Functions easing = Functions::easeLinear)
                                      :
                                      _initcolor(initcolor), _endcolor(endcolor),
                                      _start(start),
                                      _duration(60.0f * duration / TracksStatic::bpmController->currentBpm),
                                      _event(eventType),
                                      _easing(easing) {}

        [[nodiscard]] constexpr Sombrero::FastColor Interpolate(bool& modified, float songTime) const;
    };


}



DECLARE_CLASS_CODEGEN(Chroma, ChromaGradientController, UnityEngine::MonoBehaviour,
    private:
        static Chroma::ChromaGradientController* _instance;
    public:
        // internal
        gradientMap Gradients;

        static Sombrero::FastColor AddGradient(ChromaEventData::GradientObjectData const& gradientObject, GlobalNamespace::BasicBeatmapEventType id, float time);
        DECLARE_STATIC_METHOD(Chroma::ChromaGradientController*, getInstance);
        DECLARE_STATIC_METHOD(void, clearInstance);

        DECLARE_STATIC_METHOD(bool, IsGradientActive, GlobalNamespace::BasicBeatmapEventType eventType);
        DECLARE_STATIC_METHOD(void, CancelGradient, GlobalNamespace::BasicBeatmapEventType eventType);
        DECLARE_INSTANCE_METHOD(void, Update);
        DECLARE_CTOR(ctor);
        DECLARE_INSTANCE_METHOD(void, OnDestroy);
        DECLARE_SIMPLE_DTOR();
)
