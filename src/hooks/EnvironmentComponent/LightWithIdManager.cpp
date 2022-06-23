#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "GlobalNamespace/LightWithIdManager.hpp"
#include "colorizer/LightColorizer.hpp"


using namespace GlobalNamespace;
using namespace Chroma;

//MAKE_HOOK_MATCH(LightWithIdManager_RegisterLight, &LightWithIdManager::RegisterLight, void,LightWithIdManager* self, GlobalNamespace::ILightWithId* lightWithId) {
//    // Do nothing if Chroma shouldn't run
//    if (!ChromaController::DoChromaHooks()) {
//        return LightWithIdManager_RegisterLight(self, lightWithId);
//    }
//
//    std::string sceneName = self->get_gameObject()->get_scene().get_name();
//    if (sceneName.find("Menu") != std::string::npos) {
//        return LightWithIdManager_RegisterLight(self, lightWithId);
//    }
//
//    if (lightWithId->get_isRegistered()) {
//        return;
//    }
//
//    auto lightId = lightWithId->get_lightId();
//    if (lightId == -1) {
//        return;
//    }
//
//    auto& lights = self->lights[lightId];
//    if (lights == nullptr) {
//        lights = System::Collections::Generic::List_1<::GlobalNamespace::ILightWithId*>::New_ctor(10);
//    }
//
//    lightWithId->__SetIsRegistered();
//
//    if (lights->Contains(lightWithId)) {
//        return;
//    }
//
//    auto index = lights->get_Count();
//    if (Chroma::LightColorizer::NeedToRegister.contains(lightWithId)) {
//        std::optional<int> tableId;
//        auto it = Chroma::LightColorizer::RequestedIDs.find(lightWithId);
//        if (it != Chroma::LightColorizer::RequestedIDs.end()) {
//            tableId = it->second;
//        }
//        LightIDTableManager::RegisterLight(lightId, index, tableId);
//    }
//}

MAKE_HOOK_MATCH(LightWithIdManager_UnregisterLight, &LightWithIdManager::UnregisterLight, void,LightWithIdManager* self, GlobalNamespace::ILightWithId* lightWithId) {
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return LightWithIdManager_UnregisterLight(self, lightWithId);
    }

    lightWithId->__SetIsUnRegistered();
}

MAKE_HOOK_MATCH(LightWithIdManager_LateUpdate, &LightWithIdManager::LateUpdate, void,LightWithIdManager* self) {
    LightWithIdManager_LateUpdate(self);
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }
    using LightList = System::Collections::Generic::List_1<GlobalNamespace::ILightWithId *>;

    static const MethodInfo *Clear_info = il2cpp_utils::FindMethod(self->lightsToUnregister, "Clear", std::vector<Il2CppClass*>{}, ::std::vector<const Il2CppType*>{});
    static auto Clear = reinterpret_cast<void (*)(LightList *)>(Clear_info->methodPointer);

    // For some reason this doesnt get emptied and is continuously looped over.
    // When unregistering a large amount of lights in Chroma, this can add lag, so we dump the list ourselves.
    Clear(self->lightsToUnregister);
}

void LightWithIdManagerHook(Logger& logger) {
    INSTALL_HOOK(logger, LightWithIdManager_LateUpdate);
    INSTALL_HOOK(logger, LightWithIdManager_UnregisterLight);
}

ChromaInstallHooks(LightWithIdManagerHook)