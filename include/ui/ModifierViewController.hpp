#pragma once

#include "UnityEngine/MonoBehaviour.hpp"

#include <utility>
#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"

namespace Chroma {
namespace UIUtils {
UnityEngine::Sprite* configToIcon(ConfigUtils::ConfigValue<bool> const& configValue);
} // namespace UIUtils
} // namespace Chroma

DECLARE_CLASS_CODEGEN(Chroma, ModifierViewController, UnityEngine::MonoBehaviour,

                      DECLARE_INSTANCE_METHOD(void, DidActivate, bool first);

                      public
                      : DECLARE_SIMPLE_DTOR();)
