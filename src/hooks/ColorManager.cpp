#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "colorizer/NoteColorizer.hpp"
#include "colorizer/SaberColorizer.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "GlobalNamespace/ColorType.hpp"
#include "GlobalNamespace/SaberType.hpp"

using namespace GlobalNamespace;
using namespace Chroma;

MAKE_HOOK_OFFSETLESS(ColorManager_ColorForType, UnityEngine::Color, GlobalNamespace::ColorManager* self, GlobalNamespace::ColorType type) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return ColorManager_ColorForType(self, type);
    }

    if (type == ColorType::ColorA || type == ColorType::ColorB){
        auto color = NoteColorizer::getNoteColorOverride(type);

        if (color) {
            return color.value();
        }
    }

    return ColorManager_ColorForType(self, type);
}

MAKE_HOOK_OFFSETLESS(ColorManager_ColorForSaberType, UnityEngine::Color, GlobalNamespace::ColorManager* self, GlobalNamespace::SaberType type) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return ColorManager_ColorForSaberType(self, type);
    }

    auto color = SaberColorizer::SaberColorOverride[(int)type];
    if (color)
    {
        return color.value();
    }

    return ColorManager_ColorForSaberType(self, type);
}

MAKE_HOOK_OFFSETLESS(ColorManager_EffectsColorForSaberType, UnityEngine::Color, GlobalNamespace::ColorManager* self, GlobalNamespace::SaberType type) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return ColorManager_EffectsColorForSaberType(self, type);
    }

    auto color = self->ColorForSaberType(type);

    float h;
    float s;
    float ignored;

    UnityEngine::Color::RGBToHSV(color, h, s, ignored);

    return UnityEngine::Color::HSVToRGB(h, s, 1);
}

void Chroma::Hooks::ColorManager() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), ColorManager_ColorForType, il2cpp_utils::FindMethodUnsafe("", "ColorManager", "ColorForType", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), ColorManager_ColorForSaberType, il2cpp_utils::FindMethodUnsafe("", "ColorManager", "ColorForSaberType", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), ColorManager_EffectsColorForSaberType, il2cpp_utils::FindMethodUnsafe("", "ColorManager", "EffectsColorForSaberType", 1));

}