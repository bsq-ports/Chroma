#include "Chroma.hpp"

#include "ChromaController.hpp"

#include "colorizer/NoteColorizer.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "GlobalNamespace/ColorType.hpp"
#include "GlobalNamespace/SaberType.hpp"

using namespace GlobalNamespace;
using namespace Chroma;

MAKE_HOOK_OFFSETLESS(ColorManager_ColorForType, UnityEngine::Color, ColorManager* self, ColorType type) {
    if (type == ColorType::ColorA || type == ColorType::ColorB){
        auto color = NoteColorizer::getNoteColorOverride(type);

        if (color) {
            return color.value();
        }
    }

    return ColorManager_ColorForType(self, type);
}

MAKE_HOOK_OFFSETLESS(ColorManager_EffectsColorForSaberType, UnityEngine::Color, ColorManager* self, SaberType type) {
    auto color = self->ColorForSaberType(type);

    float h;
    float s;
    float ignored;

    UnityEngine::Color::RGBToHSV(color, h, s, ignored);

    return UnityEngine::Color::HSVToRGB(h, s, 1);
}

void Chroma::Hooks::ColorManager() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), ColorManager_ColorForType, il2cpp_utils::FindMethodUnsafe("", "ColorManager", "ColorForType", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), ColorManager_EffectsColorForSaberType, il2cpp_utils::FindMethodUnsafe("", "ColorManager", "EffectsColorForSaberType", 1));

}