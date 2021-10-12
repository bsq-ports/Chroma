#include "hooks/LightSwitchEventEffect.hpp"
#include "lighting/ChromaGradientController.hpp"

#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"

#include "ChromaController.hpp"
#include "UnityEngine/Color.hpp"
#include "utils/ChromaUtils.hpp"
#include "Chroma.hpp"

#include "colorizer/LightColorizer.hpp"
#include "UnityEngine/GameObject.hpp"

#include "utils/ChromaAudioTimeSourceHelper.hpp"

DEFINE_TYPE(Chroma, ChromaGradientController);

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;

//std::unordered_map<GlobalNamespace::BeatmapEventType, ChromaGradientEvent*> Chroma::ChromaGradientController::gradients = std::unordered_map<GlobalNamespace::BeatmapEventType, ChromaGradientEvent*>();

Chroma::ChromaGradientController* ChromaGradientController::_instance = nullptr;

void ChromaGradientController::ctor() {
    Gradients = gradientMap();
}

Chroma::ChromaGradientController* ChromaGradientController::getInstance() {
    static auto str = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("Chroma_GradientController");
    if (_instance == nullptr)
    {
        _instance = UnityEngine::GameObject::New_ctor(str)->AddComponent<ChromaGradientController*>();
    }

    return _instance;
}

void Chroma::ChromaGradientController::clearInstance() {
    if (_instance) {
        UnityEngine::GameObject::Destroy(_instance);
        _instance = nullptr;
    }
}

void ChromaGradientController::OnDestroy() {
    _instance = nullptr;
}


bool Chroma::ChromaGradientController::IsGradientActive(GlobalNamespace::BeatmapEventType eventType) {
    return getInstance()->Gradients.contains(eventType);
}

void ChromaGradientController::CancelGradient(GlobalNamespace::BeatmapEventType eventType) {
    getInstance()->Gradients.erase(eventType);
}

Sombrero::FastColor ChromaGradientController::AddGradient(ChromaEventData::GradientObjectData const& gradientObject,
                                                         GlobalNamespace::BeatmapEventType id, float time) {
    CancelGradient(id);

    float duration = gradientObject.Duration;
    Sombrero::FastColor const& initcolor = gradientObject.StartColor;
    Sombrero::FastColor const& endcolor = gradientObject.EndColor;
    Functions easing = gradientObject.Easing;

    auto it = getInstance()->Gradients.emplace(id.value, ChromaGradientEvent(initcolor, endcolor, time, duration, id, easing));
    // Grab by reference since assignment copies to the map
    // This way calling interpolate actually modifies the struct itself.
    auto& newGradientEvent = it.first->second;
    bool erased = false;


    auto r = newGradientEvent.Interpolate(erased,
                                          ChromaTimeSourceHelper::getSongTimeChroma(ChromaController::IAudioTimeSource));

    if (erased)
        getInstance()->Gradients.erase(it.first);

    return r;
}

void Chroma::ChromaGradientController::Update() {
    if (!Gradients.empty()) {
        // Create a map iterator and point to beginning of map
        auto it = Gradients.begin();

        auto songTime = ChromaTimeSourceHelper::getSongTimeChroma(ChromaController::IAudioTimeSource);
        // Iterate over the map using Iterator till end.
        while (it != Gradients.end()) {

            bool modified = false;
            // Accessing KEY from element pointed by it.
            BeatmapEventType eventType = it->first;

            // Accessing VALUE from element pointed by it.
            Sombrero::FastColor color = it->second.Interpolate(modified, songTime);

            LightColorizer::ColorizeLight(eventType, true, {color, color, color, color});

            if (!modified)
                it++;
            else
                it = Gradients.erase(it);
        }
    }
}

static constexpr Sombrero::FastColor lerpUnclamped(Sombrero::FastColor const& a, Sombrero::FastColor const& b, float t) {
    return {a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t, a.a + (b.a - a.a) * t};
}

constexpr Sombrero::FastColor Chroma::ChromaGradientEvent::Interpolate(bool& modified, const float songTime) const {
    modified = false;
    float normalTime = songTime - _start;
    if (normalTime < 0)
    {
        return _initcolor;
    }
    else if (normalTime <= _duration)
    {
        return lerpUnclamped(_initcolor, _endcolor, Easings::Interpolate(normalTime / _duration, _easing));
    }
    else
    {
        modified = true;
        return _endcolor;
    }
}

