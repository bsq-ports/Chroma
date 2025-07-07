#include "main.hpp"
#include "BombAPI.hpp"
#include "colorizer/BombColorizer.hpp"

#include "GlobalNamespace/ColorType.hpp"
#include "GlobalNamespace/NoteData.hpp"

#include "conditional-dependencies/shared/main.hpp"
#include "shared/utils.hpp"
#include "ChromaObjectData.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Sombrero;

// TODO: unsure of this
EXPOSE_API(getBombColorSafe, OptColor) {
  auto optional = BombColorizer::getGlobalColorStatic();

  Sombrero::FastColor color;

  if (optional) {

    color = optional.value();

    return OptColorFromColor(color);
  }
  return OptColorNull;
}

EXPOSE_API(getBombNoteControllerOverrideColorSafe, OptColor, BombNoteController* BombNoteController) {
  auto* cnv = BombColorizer::GetBombColorizer(BombNoteController);

  if (cnv == nullptr) {
    return {};
  }

  auto color = cnv->getColor();

  return OptColorFromColor(color);
}

EXPOSE_API(getBombNoteControllerColorSafe, OptColor, BombNoteController* BombNoteController) {
  auto it = ChromaObjectDataManager::ChromaObjectDatas.find(BombNoteController->noteData);

  if (it == ChromaObjectDataManager::ChromaObjectDatas.end()) {
    return OptColorNull;
  }

  auto color = it->second.Color;

  if (!color) {
    return OptColorNull;
  }

  return OptColorFromColor(color.value());
}

EXPOSE_API(setBombColorSafe, void, BombNoteController* nc, std::optional<Sombrero::FastColor> color0) {
  BombColorizer::ColorizeBomb(nc, color0);
}

EXPOSE_API(setGlobalBombColorSafe, void, std::optional<Sombrero::FastColor> color0) {
  BombColorizer::GlobalColorize(color0);
}

EXPOSE_API(setBombColorable, void, bool colorable) {
  BombColorizer::BombColorable = colorable;
}

EXPOSE_API(isBombColorable, bool) {
  return BombColorizer::BombColorable;
}
EXPOSE_API(getBombChangedColorCallbackSafe, BombAPI::BombCallback*) {
  return &BombColorizer::BombColorChanged;
}