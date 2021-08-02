#include "main.hpp"
#include "NoteAPI.hpp"
#include "colorizer/NoteColorizer.hpp"

#include "conditional-dependencies/shared/main.hpp"
#include "shared/utils.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Sombrero;

// TODO: unsure of this
EXPOSE_API(getNoteColorSafe, OptColor, int colorType) {
    CRASH_UNLESS(colorType >= ColorType::ColorA && colorType <= ColorType::ColorB);
    auto optional = NoteColorizer::GlobalColor[colorType];

    Sombrero::FastColor color;

    if (optional) {

        color = optional.value();

        return OptColorFromColor(color);
    } else {
        return OptColorNull;
    }
}

EXPOSE_API(getNoteControllerOverrideColorSafe, OptColor, NoteController* noteController, int colorType) {
    CRASH_UNLESS(colorType >= ColorType::ColorA && colorType <= ColorType::ColorB);

    auto cnv = NoteColorizer::GetNoteColorizer(noteController);

    if (!cnv) return OptColor();

    auto color = cnv->getColor();

    return OptColorFromColor(color);
}

EXPOSE_API(getNoteControllerColorSafe, OptColor, NoteController* noteController, int colorType) {
    CRASH_UNLESS(colorType >= ColorType::ColorA && colorType <= ColorType::ColorB);

    auto it = ChromaObjectDataManager::ChromaObjectDatas.find(noteController->noteData);

    if (it == ChromaObjectDataManager::ChromaObjectDatas.end())
        return OptColorNull;

    auto color = it->second->Color;

    if (!color)
        return OptColorNull;

    return OptColorFromColor(color.value());
}

EXPOSE_API(setNoteColorSafe, void, NoteControllerBase* nc, std::optional<Sombrero::FastColor> color0) {
    NoteColorizer::ColorizeNote(nc, color0);
}

extern "C" void __setNoteColorable(bool colorable) {
    NoteColorizer::NoteColorable = colorable;
}

extern "C" bool __isNoteColorable() {
    return NoteColorizer::NoteColorable;
}
