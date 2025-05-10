#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/BombColorizer.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "utils/ChromaUtils.hpp"

#include "ChromaObjectData.hpp"

#include "GlobalNamespace/BombNoteController.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;

MAKE_HOOK_MATCH(BombNoteController_Init, &BombNoteController::Init, void, BombNoteController* self,
                ::GlobalNamespace::NoteData* noteData, ::ByRef<::GlobalNamespace::NoteSpawnData> noteSpawnData) {
  BombNoteController_Init(self, noteData, noteSpawnData);
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return;
  }

  auto chromaData = ChromaObjectDataManager::ChromaObjectDatas.find(noteData);
  if (chromaData != ChromaObjectDataManager::ChromaObjectDatas.end()) {
    auto const& color = chromaData->second.Color;

    BombColorizer::ColorizeBomb(self, color);
  }
}

void BombNoteControllerHook() {
  INSTALL_HOOK(ChromaLogger::Logger, BombNoteController_Init);
}

ChromaInstallHooks(BombNoteControllerHook)