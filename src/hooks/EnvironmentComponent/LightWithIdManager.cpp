#include "ChromaController.hpp"

#include "GlobalNamespace/LightWithIdManager.hpp"
#include "colorizer/LightColorizer.hpp"
#include "custom-json-data/shared/VList.h"

#include "hooks/LightWithIdManager.hpp"

std::unordered_map<GlobalNamespace::ILightWithId*, int> Chroma::LightIdRegisterer::RequestedIDs;
std::unordered_set<GlobalNamespace::ILightWithId*> Chroma::LightIdRegisterer::NeedToRegister;
GlobalNamespace::LightWithIdManager* Chroma::LightIdRegisterer::lightWithIdManager;
bool Chroma::LightIdRegisterer::canUnregister = false;

using namespace GlobalNamespace;
using namespace Chroma;

MAKE_HOOK_MATCH(LightWithIdManager_RegisterLight, &LightWithIdManager::RegisterLight, void, LightWithIdManager* self,
                GlobalNamespace::ILightWithId* lightWithId) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
    return LightWithIdManager_RegisterLight(self, lightWithId);
  }

  std::string sceneName = self->get_gameObject()->get_scene().get_name();
  if (sceneName.find("Menu") != std::string::npos) {
    return LightWithIdManager_RegisterLight(self, lightWithId);
  }

  LightIdRegisterer::lightWithIdManager = self;

  if (lightWithId->get_isRegistered()) {
    return;
  }

  auto lightId = lightWithId->get_lightId();
  if (lightId == -1) {
    return;
  }

  // [] does not bound check, use get()
  auto& lights = self->lights.get(lightId);
  if (lights == nullptr) {
    lights = System::Collections::Generic::List_1<::GlobalNamespace::ILightWithId*>::New_ctor(10);
  }

  lightWithId->__SetIsRegistered();

  auto* lightWithIdIt = std::find(lights->items.begin(), lights->items.end(), lightWithId);
  if (lightWithIdIt != lights->items.end()) {
    return;
  }

  auto index = lights->get_Count();

  auto it = LightIdRegisterer::NeedToRegister.find(lightWithId);
  if (it != LightIdRegisterer::NeedToRegister.end()) {
    LightIdRegisterer::NeedToRegister.erase(it);

    std::optional<int> tableId;
    auto it2 = LightIdRegisterer::RequestedIDs.find(lightWithId);
    if (it2 != LightIdRegisterer::RequestedIDs.end()) {
      tableId = it2->second;
    }
    LightIDTableManager::RegisterIndex(lightId, index, tableId);
  }

  // this also colors the light
  LightColorizer::CreateLightColorizerContractByLightID(lightId, [index, lightWithId](LightColorizer& n) {
    n._lightSwitchEventEffect->RegisterLight(lightWithId, index);
  });

  lights->Add(lightWithId);
}

// This breaks if it doesn't run after Awake
// PC uses Affinity Patches which run after Awake
MAKE_HOOK_MATCH(LightWithIdManager_UnregisterLight, &LightWithIdManager::UnregisterLight, void,
                LightWithIdManager* self, GlobalNamespace::ILightWithId* lightWithId) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks() || !Chroma::LightIdRegisterer::canUnregister) {
    return LightWithIdManager_UnregisterLight(self, lightWithId);
  }

  lightWithId->__SetIsUnRegistered();
}

MAKE_HOOK_MATCH(LightWithIdManager_LateUpdate, &LightWithIdManager::LateUpdate, void, LightWithIdManager* self) {
  LightWithIdManager_LateUpdate(self);
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return;
  }

  LightIdRegisterer::lightWithIdManager = self;
  //    LightWithIdManager_LateUpdate(self);

  //    using LightList = System::Collections::Generic::List_1<GlobalNamespace::ILightWithId *>;
  //
  //    static const MethodInfo *Clear_info = il2cpp_utils::FindMethod(self->lightsToUnregister, "Clear",
  //    std::vector<Il2CppClass*>{}, ::std::vector<const Il2CppType*>{}); static auto Clear = reinterpret_cast<void
  //    (*)(LightList *)>(Clear_info->methodPointer);
  //
  //    // For some reason this doesnt get emptied and is continuously looped over.
  //    // When unregistering a large amount of lights in Chroma, this can add lag, so we dump the list ourselves.
  //    Clear(self->lightsToUnregister);
}

MAKE_HOOK_MATCH(LightWithIdManager_SetColorForId, &LightWithIdManager::SetColorForId, void, LightWithIdManager* self,
                int lightId, ::UnityEngine::Color color) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return LightWithIdManager_SetColorForId(self, lightId, color);
  }

  std::string sceneName = self->get_gameObject()->get_scene().get_name();
  if (sceneName.find("Menu") != std::string::npos) {
    return LightWithIdManager_SetColorForId(self, lightId, color);
  }

  self->colors[lightId] = { color, true };
  self->didChangeSomeColorsThisFrame = true;
  auto list = VList(self->lights.get(lightId));
  if (list == nullptr) {
    return;
  }
  for (auto const& lightWithId : list) {
    if ((lightWithId != nullptr) && lightWithId->get_isRegistered()) {
      lightWithId->ColorWasSet(color);
    }
  }
}

void LightWithIdManagerHook(Logger& logger) {
  INSTALL_HOOK(logger, LightWithIdManager_LateUpdate);
  INSTALL_HOOK(logger, LightWithIdManager_UnregisterLight);
  INSTALL_HOOK(logger, LightWithIdManager_SetColorForId);
  INSTALL_HOOK(logger, LightWithIdManager_RegisterLight);
}

ChromaInstallHooks(LightWithIdManagerHook)