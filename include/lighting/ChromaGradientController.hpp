#pragma once
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>

#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Color.hpp"

#include "GlobalNamespace/BeatmapEventType.hpp"

#include "utils/Easing.hpp"
#include "main.hpp"

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "ChromaEventData.hpp"

namespace Chroma {
    struct ChromaGradientEvent;
}

typedef std::unordered_map<int, Chroma::ChromaGradientEvent> gradientMap;

namespace Chroma {
    struct ChromaGradientEvent {
        UnityEngine::Color _initcolor;
        UnityEngine::Color _endcolor;
        float _start;
        float _duration;
        GlobalNamespace::BeatmapEventType _event;
        ChromaUtils::Functions _easing;
        std::optional<std::vector<int>> _lightIds;

        ChromaGradientEvent(UnityEngine::Color initcolor, UnityEngine::Color endcolor, float start, float duration, GlobalNamespace::BeatmapEventType eventType, std::optional<std::vector<int>> lightIds, ChromaUtils::Functions easing = ChromaUtils::Functions::easeLinear);

        [[nodiscard]] UnityEngine::Color Interpolate(bool &modified, const float& songTime) const;
    };


}



DECLARE_CLASS_CODEGEN(Chroma, ChromaGradientController, UnityEngine::MonoBehaviour,
    private:
        static Chroma::ChromaGradientController* _instance;
    public:
        // internal
        gradientMap Gradients;

        static UnityEngine::Color AddGradient(ChromaEventData::GradientObjectData gradientObject, GlobalNamespace::BeatmapEventType id, float time, std::optional<std::vector<int>> lightIds);
        DECLARE_STATIC_METHOD(Chroma::ChromaGradientController*, getInstance);
        DECLARE_STATIC_METHOD(void, clearInstance);

        DECLARE_STATIC_METHOD(bool, IsGradientActive, GlobalNamespace::BeatmapEventType eventType);
        DECLARE_STATIC_METHOD(void, CancelGradient, GlobalNamespace::BeatmapEventType eventType);
        DECLARE_INSTANCE_METHOD(void, Update);
        DECLARE_CTOR(ctor);
        DECLARE_INSTANCE_METHOD(void, OnDestroy);
        DECLARE_SIMPLE_DTOR();
)
