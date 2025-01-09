#pragma once

#include "HMUI/ViewController.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include <utility>
#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"

#include "bsml/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Chroma, ModifierViewController, HMUI::ViewController) {

  DECLARE_INSTANCE_METHOD(void, DidActivate, bool first);
  DECLARE_OVERRIDE_METHOD(void, DidActivate2,
                          il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidActivate", 3),
                          bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

  DECLARE_BSML_PROPERTY(bool, environmentEnhancementsEnabled);
  DECLARE_BSML_PROPERTY(bool, customColorEventsEnabled);
  DECLARE_BSML_PROPERTY(bool, customNoteColors);

public:
  DECLARE_SIMPLE_DTOR();
};
