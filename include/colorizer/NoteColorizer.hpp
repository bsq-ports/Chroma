#pragma once

#include "UnityEngine/MonoBehaviour.hpp"

#include "GlobalNamespace/ColorType.hpp"

#include <vector>
#include <string>
#include <optional>
#include <stack>
#include <tuple>

#include "ChromaObjectData.hpp"
#include "ObjectColorizer.hpp"

namespace GlobalNamespace {
class NoteControllerBase;
class NoteController;
class NoteCutInfo;
class MaterialPropertyBlockController;
class ColorNoteVisuals;
} // namespace GlobalNamespace

using ColorPair = std::pair<std::optional<Sombrero::FastColor>, std::optional<Sombrero::FastColor>>;
using NoteColorStack = std::stack<ColorPair>;

namespace Chroma {
class NoteColorizer : public ObjectColorizer {
  friend class std::pair<GlobalNamespace::NoteControllerBase const*, NoteColorizer>;
  friend class std::pair<GlobalNamespace::NoteControllerBase const* const, NoteColorizer>;

private:
  GlobalNamespace::NoteControllerBase* _noteController;

  GlobalNamespace::ColorNoteVisuals* _colorNoteVisuals;
  ArrayW<UnityW<GlobalNamespace::MaterialPropertyBlockController>> _materialPropertyBlockControllers;

  NoteColorizer(GlobalNamespace::NoteControllerBase* noteController);

protected:
  [[nodiscard]] std::optional<Sombrero::FastColor> getGlobalColor() const final;

  [[nodiscard]] Sombrero::FastColor getOriginalColor() const final;

  void Refresh() final;

public:
  inline static bool NoteColorable = false;
  inline static UnorderedEventCallback<GlobalNamespace::NoteControllerBase*, Sombrero::FastColor const&,
                                       GlobalNamespace::ColorType>
      NoteColorChanged;

  inline static std::unordered_map<GlobalNamespace::NoteControllerBase const*, NoteColorizer> Colorizers;
  inline static std::array<std::optional<Sombrero::FastColor>, 2> GlobalColor = { std::nullopt, std::nullopt };

  static NoteColorizer* New(GlobalNamespace::NoteControllerBase* noteControllerBase);
  static void GlobalColorize(std::optional<Sombrero::FastColor> const& color,
                             GlobalNamespace::ColorType const& colorType);
  static void Reset();
  static void ColorizeSaber(GlobalNamespace::NoteController* noteController,
                            GlobalNamespace::NoteCutInfo const& noteCutInfo);

  [[nodiscard]] GlobalNamespace::ColorType getColorType() const;

  // extensions
  static NoteColorizer* GetNoteColorizer(GlobalNamespace::NoteControllerBase* noteController) {
    auto it = Colorizers.find(noteController);
    if (it == Colorizers.end()) {
      return nullptr;
    }

    return &it->second;
  }

  constexpr static void ColorizeNote(GlobalNamespace::NoteControllerBase* noteController,
                                     std::optional<Sombrero::FastColor> const& color) {
    auto* colorizer = GetNoteColorizer(noteController);
    if (colorizer != nullptr) {
      colorizer->Colorize(color);
    }
  }
};
} // namespace Chroma
