#include "ChromaGradientController.hpp"

#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"

#include "ChromaController.hpp"
#include "UnityEngine/Color.hpp"
#include "utils/Easing.hpp"
#include "utils/ChromaUtils.hpp"
#include "Chroma.hpp"

#include "colorizer/LightColorizer.hpp"
#include "UnityEngine/GameObject.hpp"

DEFINE_CLASS(Chroma::ChromaGradientController);

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;

//std::unordered_map<GlobalNamespace::BeatmapEventType, ChromaGradientEvent*> Chroma::ChromaGradientController::gradients = std::unordered_map<GlobalNamespace::BeatmapEventType, ChromaGradientEvent*>();

Chroma::ChromaGradientController* ChromaGradientController::getInstance() {
    if (_instance == nullptr)
    {
        _instance = UnityEngine::GameObject::New_ctor(il2cpp_utils::createcsstr("Chroma_GradientController"))->AddComponent<ChromaGradientController*>();
    }

    return _instance;
}

bool Chroma::ChromaGradientController::IsGradientActive(GlobalNamespace::BeatmapEventType eventType) {
    return Gradients.find(eventType) != Gradients.end();
}

void ChromaGradientController::CancelGradient(GlobalNamespace::BeatmapEventType eventType) {
    Gradients.erase(eventType);
}

UnityEngine::Color ChromaGradientController::AddGradient(rapidjson::Value* gradientObject,
                                                         GlobalNamespace::BeatmapEventType id, float time) {
    CancelGradient(id);

    float duration = gradientObject->FindMember(Chroma::DURATION)->value.GetFloat(); // Trees.at(gradientObject, DURATION);
    UnityEngine::Color initcolor = ChromaUtils::ChromaUtilities::GetColorFromData(gradientObject, STARTCOLOR).value();
    UnityEngine::Color endcolor = ChromaUtils::ChromaUtilities::GetColorFromData(gradientObject, ENDCOLOR).value();
    std::string easingString = std::string(gradientObject->FindMember(EASING)->value.GetString());
    ChromaUtils::Functions easing;

    if (easingString.empty())
    {
        easing = ChromaUtils::Functions::easeLinear;

    } else {
        auto s = ChromaUtils::FUNCTION_NAMES;

        easing = (ChromaUtils::Functions) s[easingString];
    }

    auto* gradientEvent = new ChromaGradientEvent(initcolor, endcolor, time, duration, id, easing);
    Gradients[id.value] = gradientEvent;
    return gradientEvent->Interpolate();
}

void Chroma::ChromaGradientController::Update() {
    // Create a map iterator and point to beginning of map
    auto it = Gradients.begin();

    // Iterate over the map using Iterator till end.
    while (it != Gradients.end()) {
        // Accessing KEY from element pointed by it.
        BeatmapEventType eventType = it->first;
        // Accessing VALUE from element pointed by it.
        UnityEngine::Color color = it->second->Interpolate();

        LightColorizer::SetLightingColors(eventType, color, color, color, color);
        LightColorizer::SetActiveColors(eventType);

        it++;
    }
}

Chroma::ChromaGradientEvent::ChromaGradientEvent(UnityEngine::Color initcolor, UnityEngine::Color endcolor, float start,
                                                 float duration, GlobalNamespace::BeatmapEventType eventType,
                                                 ChromaUtils::Functions easing) {
    _initcolor = initcolor;
    _endcolor = endcolor;
    _start = start;
    _duration = 60.0f * duration / ChromaController::BeatmapObjectSpawnController->get_currentBpm();
    _event = eventType;
    _easing = easing;
}

UnityEngine::Color lerpUnclamped(UnityEngine::Color a, UnityEngine::Color b, float t) {
    return UnityEngine::Color(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t, a.a + (b.a - a.a) * t);
}

UnityEngine::Color Chroma::ChromaGradientEvent::Interpolate() const {
    float normalTime = ChromaController::IAudioTimeSource->get_songTime() - _start;
    if (normalTime < 0)
    {
        return _initcolor;
    }
    else if (normalTime <= _duration)
    {
        return lerpUnclamped(_initcolor, _endcolor, ChromaUtils::Easings::Interpolate(normalTime / _duration, _easing));
    }
    else
    {
        Chroma::Gradients.erase(_event.value);
        return _endcolor;
    }
}

