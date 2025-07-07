#pragma once

#include "ObjectColorizer.hpp"

#include "UnityEngine/MonoBehaviour.hpp"

#include "custom-types/shared/macros.hpp"

#include <optional>

namespace GlobalNamespace {
class NoteControllerBase;
class MaterialPropertyBlockController;
} // namespace GlobalNamespace

namespace Chroma {
class BombColorizer : public ObjectColorizer {
private:
  GlobalNamespace::MaterialPropertyBlockController* _materialPropertyBlockController;

  GlobalNamespace::NoteControllerBase* noteController;

  inline static std::optional<Sombrero::FastColor> GlobalColor;

  explicit BombColorizer(GlobalNamespace::NoteControllerBase* noteController);

protected:
 [[nodiscard]] std::optional<Sombrero::FastColor> getGlobalColor() const final;

public:
  inline static bool BombColorable = false;
  inline static UnorderedEventCallback<GlobalNamespace::NoteControllerBase*, Sombrero::FastColor const&> BombColorChanged;

  BombColorizer(BombColorizer const&) = delete;
  friend class std::pair<GlobalNamespace::NoteControllerBase const*, BombColorizer>;
  friend class std::pair<GlobalNamespace::NoteControllerBase const* const, BombColorizer>;

  static BombColorizer& New(GlobalNamespace::NoteControllerBase* noteController);

  using ColorizerMap = std::unordered_map<GlobalNamespace::NoteControllerBase const*, BombColorizer>;
  static ColorizerMap Colorizers;

  static std::optional<Sombrero::FastColor> getGlobalColorStatic();

  static void GlobalColorize(std::optional<Sombrero::FastColor> const& color);

  static void Reset();

  void Refresh() final;

  // extensions
  inline static BombColorizer* GetBombColorizer(GlobalNamespace::NoteControllerBase* noteController) {
    auto it = Colorizers.find(noteController);
    if (it == Colorizers.end()) {
      return nullptr;
    }

    return &it->second;
  }

  inline static void ColorizeBomb(GlobalNamespace::NoteControllerBase* noteController, std::optional<Sombrero::FastColor> const& color) {
    CRASH_UNLESS(GetBombColorizer(noteController))->Colorize(color);
  }
};
} // namespace Chroma
