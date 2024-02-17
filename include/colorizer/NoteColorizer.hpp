#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Shader.hpp"

#include "GlobalNamespace/NoteControllerBase.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ColorNoteVisuals.hpp"
#include "GlobalNamespace/ColorManager.hpp"

#include <vector>
#include <string>
#include <optional>
#include <stack>
#include <tuple>

#include "ChromaObjectData.hpp"
#include "ObjectColorizer.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

using ColorPair = std::pair<std::optional<Sombrero::FastColor>, std::optional<Sombrero::FastColor>>;
using NoteColorStack = std::stack<ColorPair>;

namespace Chroma {
class NoteColorizer : public ObjectColorizer<NoteColorizer> {
private:
  friend class ObjectColorizer<NoteColorizer>;
  static int _colorID();

  GlobalNamespace::NoteControllerBase* _noteController;

  GlobalNamespace::ColorNoteVisuals* _colorNoteVisuals;
  ArrayW<UnityW<GlobalNamespace::MaterialPropertyBlockController>> _materialPropertyBlockControllers;

  NoteColorizer(GlobalNamespace::NoteControllerBase* noteController);

protected:

  std::optional<Sombrero::FastColor> GlobalColorGetter();

  std::optional<Sombrero::FastColor> OriginalColorGetter();

  void Refresh();

public:
  inline static bool NoteColorable = false;
  inline static UnorderedEventCallback<GlobalNamespace::NoteControllerBase*, Sombrero::FastColor const&,
                                       GlobalNamespace::ColorType>
      NoteColorChanged;
  inline static std::unordered_map<GlobalNamespace::NoteControllerBase const*, NoteColorizer> Colorizers;
  inline static std::array<std::optional<Sombrero::FastColor>, 2> GlobalColor = { std::nullopt, std::nullopt };
  GlobalNamespace::ColorType getColorType();

  friend class std::pair<GlobalNamespace::NoteControllerBase const*, NoteColorizer>;
  friend class std::pair<GlobalNamespace::NoteControllerBase const* const, Chroma::NoteColorizer>;
  NoteColorizer(NoteColorizer const&) = delete;
  static NoteColorizer* New(GlobalNamespace::NoteControllerBase* noteControllerBase);

  static void GlobalColorize(std::optional<Sombrero::FastColor> const& color,
                             GlobalNamespace::ColorType const& colorType);

  static void Reset();

  static void ColorizeSaber(GlobalNamespace::NoteController* noteController,
                            GlobalNamespace::NoteCutInfo const& noteCutInfo);

  // extensions
  inline static NoteColorizer* GetNoteColorizer(GlobalNamespace::NoteControllerBase* noteController) {
    auto it = Colorizers.find(noteController);
    if (it == Colorizers.end()) {
      return nullptr;
    }

    return &it->second;
  }

  inline static void ColorizeNote(GlobalNamespace::NoteControllerBase* noteController,
                                  std::optional<Sombrero::FastColor> const& color) {
    auto* colorizer = GetNoteColorizer(noteController);
    if (colorizer != nullptr) {
      colorizer->Colorize(color);
    }
  }
};
} // namespace Chroma
