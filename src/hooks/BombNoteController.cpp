
#include "Chroma.hpp"

#include "colorizer/BombColorizer.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/BombNoteController.hpp"
#include "utils/ChromaUtils.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;

MAKE_HOOK_OFFSETLESS(
    BombNoteController_Init,
    void,
    BombNoteController* self,
    CustomNoteData* noteData
) {
    BombColorizer::BNCStart(self);

    std::optional<UnityEngine::Color> color = std::nullopt;

    if(noteData->customData && noteData->customData->value) {
        color = ChromaUtilities::GetColorFromData(noteData->customData->value);
    }

    if (color) {
        BombColorizer::SetBombColor(self, color);
    } else {
        BombColorizer::Reset(self);
    }
}

void Chroma::Hooks::BombNoteController() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), BombNoteController_Init, il2cpp_utils::FindMethodUnsafe("", "BombNoteController", "Init", 1));
}