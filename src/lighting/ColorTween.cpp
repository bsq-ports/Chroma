#include "lighting/ColorTween.hpp"

#include "System/Action_1.hpp"

#include "custom-types/shared/delegate.hpp"


DEFINE_TYPE(Chroma, ChromaIDColorTween);

void Chroma::ChromaIDColorTween::ctor(Sombrero::FastColor fromValue, Sombrero::FastColor toValue,
                                      GlobalNamespace::ILightWithId *lightWithId,
                                      GlobalNamespace::LightWithIdManager *lightWithIdManager, int id) {
    std::function<void(UnityEngine::Color)> SetColor = [this](UnityEngine::Color const& color) constexpr {
        ChromaIDColorTween::SetColor(color);
    };
    auto SetColorAction = custom_types::MakeDelegate<System::Action_1<UnityEngine::Color>*>(SetColor);

    Init(fromValue, toValue, SetColorAction, 0, GlobalNamespace::EaseType::Linear, 0);
    _lightWithId = lightWithId;
    _lightWithIdManager = lightWithIdManager;
    Id = id;
}
