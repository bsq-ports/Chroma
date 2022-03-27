#include "lighting/ColorTween.hpp"

#include "System/Action_1.hpp"

#define MakeDelegate(DelegateType, varName) (il2cpp_utils::MakeDelegate<DelegateType>(classof(DelegateType), varName))

DEFINE_TYPE(Chroma, ChromaIDColorTween);

void Chroma::ChromaIDColorTween::ctor(Sombrero::FastColor fromValue, Sombrero::FastColor toValue,
                                      GlobalNamespace::ILightWithId *lightWithId,
                                      GlobalNamespace::LightWithIdManager *lightWithIdManager, int id) {
    std::function<void(UnityEngine::Color)> SetColor = [this](UnityEngine::Color const& color) {
        ChromaIDColorTween::SetColor(color);
    };
    auto SetColorAction = MakeDelegate(System::Action_1<UnityEngine::Color>*, SetColor);

    Reinit(fromValue, toValue, SetColorAction, 0, GlobalNamespace::EaseType::Linear, 0);
    _lightWithId = lightWithId;
    _lightWithIdManager = lightWithIdManager;
    Id = id;
}
