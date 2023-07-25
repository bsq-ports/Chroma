#pragma once

#include "GlobalNamespace/SaberManager.hpp"

namespace Chroma {
class SaberManagerHolder {
public:
  inline static GlobalNamespace::SaberManager* saberManager = nullptr;
};
} // namespace Chroma