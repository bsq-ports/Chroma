#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "GlobalNamespace/LightWithIdManager.hpp"


using namespace GlobalNamespace;
using namespace Chroma;

MAKE_HOOK_MATCH(LightWithIdManager_LateUpdate, &LightWithIdManager::LateUpdate, void,LightWithIdManager* self) {
    LightWithIdManager_LateUpdate(self);
    // Do nothing if Chroma shouldn't run
    if (!ChromaController::DoChromaHooks()) {
        return;
    }
    typedef System::Collections::Generic::List_1<GlobalNamespace::ILightWithId*> lightList;

    static const MethodInfo *Clear_info = il2cpp_utils::FindMethod(self->lightsToUnregister, "Clear", std::vector<Il2CppClass*>{}, ::std::vector<const Il2CppType*>{});
    static auto Clear = reinterpret_cast<void (*)(lightList *)>(Clear_info->methodPointer);

    // For some reason this doesnt get emptied and is continuously looped over.
    // When unregistering a large amount of lights in Chroma, this can add lag, so we dump the list ourselves.
    Clear(self->lightsToUnregister);
}

void LightWithIdManagerHook(Logger& logger) {
    INSTALL_HOOK(logger, LightWithIdManager_LateUpdate);
}

ChromaInstallHooks(LightWithIdManagerHook)