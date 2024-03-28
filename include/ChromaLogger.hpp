#pragma once
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "custom-json-data/shared/CJDLogger.h"

class ChromaLogger {
public:
  static constexpr auto Logger = Paper::ConstLoggerContext("Chroma");
};
