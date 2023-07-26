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
EXPOSE_API(getGlobalNoteColorSafe, OptColor, int colorType) {
  CRASH_UNLESS(colorType >= ColorType::ColorA && colorType <= ColorType::ColorB);
  auto optional = NoteColorizer::GlobalColor[colorType];

  if (optional) {
    return OptColorFromColor(optional.value());
  }
  return OptColorNull;
}

EXPOSE_API(getNoteControllerOverrideColorSafe, OptColor, NoteController* noteController) {
  auto* cnv = NoteColorizer::GetNoteColorizer(noteController);

  if (cnv == nullptr) {
    return {};
  }

  auto color = cnv->getColor();

  return OptColorFromColor(color);
}

EXPOSE_API(getNoteControllerColorSafe, OptColor, NoteController* noteController) {
  auto it = ChromaObjectDataManager::ChromaObjectDatas.find(noteController->noteData);

  if (it == ChromaObjectDataManager::ChromaObjectDatas.end()) {
    return OptColorNull;
  }

  auto const& color = it->second.Color;

  if (!color) {
    return OptColorNull;
  }

  return OptColorFromColor(color.value());
}

EXPOSE_API(getRealtimeNoteControllerColorSafe, OptColor, NoteController* noteController) {
  auto* colorizer = NoteColorizer::GetNoteColorizer(noteController);

  if (colorizer == nullptr) {
    return OptColorNull;
  }

  return OptColorFromColor(colorizer->getColor());
}

EXPOSE_API(setNoteColorSafe, void, NoteControllerBase* nc, std::optional<Sombrero::FastColor> color0) {
  NoteColorizer::ColorizeNote(nc, color0);
}

EXPOSE_API(setNoteColorable, void, bool colorable) {
  NoteColorizer::NoteColorable = colorable;
}

EXPOSE_API(setGlobalNoteColorSafe, void, std::optional<Sombrero::FastColor> color0,
           std::optional<Sombrero::FastColor> color1) {
  NoteColorizer::GlobalColorize(color0, ColorType::ColorA);
  NoteColorizer::GlobalColorize(color1, ColorType::ColorB);
}

EXPOSE_API(isNoteColorable, bool) {
  return NoteColorizer::NoteColorable;
}
EXPOSE_API(getNoteChangedColorCallbackSafe, NoteAPI::NoteCallback*) {
  return &NoteColorizer::NoteColorChanged;
}