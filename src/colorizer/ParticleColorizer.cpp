#include "colorizer/ParticleColorizer.hpp"
#include "colorizer/LightColorizer.hpp"
#include "GlobalNamespace/ColorExtensions.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Sombrero;

Chroma::ParticleColorizer::ParticleColorizer(GlobalNamespace::ParticleSystemEventEffect *particleSystemEventEffect,
                                             GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType) :
                                             _simpleColorSOs(COLOR_FIELDS),
                                             _multipliedColorSOs(COLOR_FIELDS),
                                             _multipliedHighlightColorSOs(COLOR_FIELDS),
                                             _particleSystemEventEffect(particleSystemEventEffect),
                                             _eventType(BasicBeatmapEventType)
                                             {
    InitializeSO("_lightColor0", 0);
    InitializeSO("_highlightColor0", 0, true);
    InitializeSO("_lightColor1", 1);
    InitializeSO("_highlightColor1", 1, true);



    LightColorizer::LightColorChanged += {&ParticleColorizer::OnLightColorChanged, this};
}

std::shared_ptr<ParticleColorizer> ParticleColorizer::New(GlobalNamespace::ParticleSystemEventEffect *particleSystemEventEffect,
                               GlobalNamespace::BasicBeatmapEventType BasicBeatmapEventType) {
    std::shared_ptr<ParticleColorizer> particleColorizer(new ParticleColorizer(particleSystemEventEffect, BasicBeatmapEventType));

    GetOrCreateColorizerList(BasicBeatmapEventType).emplace(particleColorizer);
    return particleColorizer;
}

void ParticleColorizer::UnsubscribeEvent() {
    LightColorizer::LightColorChanged -= {&ParticleColorizer::OnLightColorChanged, this};
}

std::unordered_set<std::shared_ptr<ParticleColorizer>>&
ParticleColorizer::GetOrCreateColorizerList(GlobalNamespace::BasicBeatmapEventType eventType) {
    auto it = Colorizers.find(eventType);

    if (it == Colorizers.end()) {
        Colorizers[eventType] = std::unordered_set<std::shared_ptr<ParticleColorizer>>();
        return Colorizers[eventType];
    }

    return it->second;
}

void ParticleColorizer::OnLightColorChanged(GlobalNamespace::BasicBeatmapEventType eventType,
                                            std::array<Sombrero::FastColor, 4> colors) {
    if (eventType == _eventType)
    {
        for (int i = 0; i < COLOR_FIELDS; i++)
        {
            if (i >= colors.size()) {
                // is this normal?
                break;
            }

            auto color = colors[i];

            static auto SetColor = FPtrWrapper<&GlobalNamespace::SimpleColorSO::SetColor>::get();

            auto it = _simpleColorSOs.find(i); // std::unordered_map<int, SafePtr<GlobalNamespace::SimpleColorSO>>
            if (it != _simpleColorSOs.end() && it->second && (SimpleColorSO*) it->second)
                SetColor((SimpleColorSO*) it->second, color);
        }

       static auto RefreshParticles = FPtrWrapper<&GlobalNamespace::ParticleSystemEventEffect::RefreshParticles>::get();

        auto particleSystemEventEffect = _particleSystemEventEffect;
        Sombrero::FastColor color;
        Sombrero::FastColor afterHighlightColor;
        switch (PreviousValue)
        {
            case 0:
                particleSystemEventEffect->particleColor = particleSystemEventEffect->offColor;
                RefreshParticles(particleSystemEventEffect);
                break;

            case 1:
            case 5:
                color = (PreviousValue == 1) ? _multipliedColorSOs[0]->get_color() : _multipliedColorSOs[1]->get_color();
                particleSystemEventEffect->particleColor = color;
                particleSystemEventEffect->offColor = color.Alpha(0);
                RefreshParticles(particleSystemEventEffect);
                break;

            case 2:
            case 6:
                color = (PreviousValue == 2) ? _multipliedHighlightColorSOs[0]->get_color() : _multipliedHighlightColorSOs[1]->get_color();
                particleSystemEventEffect->highlightColor = color;
                particleSystemEventEffect->offColor = color.Alpha(0);
                afterHighlightColor = (PreviousValue == 2) ? _multipliedColorSOs[0]->get_color() : _multipliedColorSOs[1]->get_color();
                particleSystemEventEffect->afterHighlightColor = afterHighlightColor;

                particleSystemEventEffect->particleColor = Sombrero::FastColor::Lerp(afterHighlightColor, color, particleSystemEventEffect->highlightValue);
                RefreshParticles(particleSystemEventEffect);
                break;

            case 3:
            case 7:
            case -1:
                color = (PreviousValue == 3) ? _multipliedHighlightColorSOs[0]->get_color() : _multipliedHighlightColorSOs[1]->get_color();
                particleSystemEventEffect->highlightColor = color;
                particleSystemEventEffect->offColor = color.Alpha(0);
                particleSystemEventEffect->particleColor = color;
                afterHighlightColor = particleSystemEventEffect->offColor;
                particleSystemEventEffect->afterHighlightColor = afterHighlightColor;

                particleSystemEventEffect->particleColor = Sombrero::FastColor::Lerp(afterHighlightColor, color, particleSystemEventEffect->highlightValue);
                RefreshParticles(particleSystemEventEffect);
                break;
        }
    }
}

void ParticleColorizer::InitializeSO(const std::string& id, int index, bool highlight) {
    static Il2CppClass* klass = classof(ParticleSystemEventEffect*);
    auto colorSOAcessor = il2cpp_utils::FindField(klass, (std::string_view) id);
    auto lightMultSO = il2cpp_utils::cast<MultipliedColorSO>(CRASH_UNLESS(il2cpp_utils::GetFieldValue<GlobalNamespace::ColorSO*>(_particleSystemEventEffect, colorSOAcessor)));

    Sombrero::FastColor multiplierColor = lightMultSO->multiplierColor;
    auto lightSO = lightMultSO->baseColor;

    SafePtrUnity<MultipliedColorSO> mColorSO(ScriptableObject::CreateInstance<MultipliedColorSO*>());
    mColorSO->multiplierColor = multiplierColor;


    if (_simpleColorSOs.find(index) == _simpleColorSOs.end())
    {
        SafePtrUnity<SimpleColorSO> sColorSO(ScriptableObject::CreateInstance<SimpleColorSO*>());
        sColorSO->SetColor(lightSO->color);
        _simpleColorSOs.emplace(index, sColorSO);
    }

    SafePtrUnity<SimpleColorSO>& sColorSO = _simpleColorSOs[index];

    mColorSO->baseColor = (SimpleColorSO*) sColorSO;
    MultipliedColorSO* mColorPtr = (MultipliedColorSO*) mColorSO;

    if (highlight)
    {
        _multipliedHighlightColorSOs.emplace(index, mColorSO);
    }
    else
    {
        _multipliedColorSOs.emplace(index, mColorSO);
    }

    il2cpp_utils::SetFieldValue<ColorSO*>(_particleSystemEventEffect, colorSOAcessor, mColorPtr);
}

void ParticleColorizer::Reset() {
    Colorizers.clear();
    Colorizers = {};
}
