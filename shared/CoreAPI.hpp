#pragma once

#include "scotland2/shared/loader.hpp"
#include "utils.hpp"
#include "conditional-dependencies/shared/main.hpp"

namespace Chroma {
class CoreAPI {
public:
  // Adds a mod that will force Chroma to enable its hooks (for colorizing), when not in a Chroma map.
  static void addForceEnableChromaHooks(modloader::ModInfo& modInfo) {
    static auto function = CondDeps::Find<void, modloader::ModInfo&>(CHROMA_ID, "addForceEnableChromaHooks");

    if (function) {
      function.value()(modInfo);
    }
  }

  // Removes a mod so that it will no longer force Chroma to enable its hooks (for colorizing), when not in a Chroma
  // map.
  static void removeForceEnableChromaHooks(modloader::ModInfo& modInfo) {
    static auto function = CondDeps::Find<void, modloader::ModInfo&>(CHROMA_ID, "removeForceEnableChromaHooks");

    if (function) {
      function.value()(modInfo);
    }
  }

  static bool isChromaRunning() {
    static auto function = CondDeps::Find<bool>(CHROMA_ID, "isChromaRunning");

    if (function) {
      return function.value()();
    }
    return false;
  }
};
} // namespace Chroma