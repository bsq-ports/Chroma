#include "Chroma.hpp"

#include "System/Collections/Generic/IEnumerator_1.hpp"
#include "colorizer/LightColorizer.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;

DECLARE_CLASS_INTERFACES(Il2CppNamespace, WaitThenStartEnumerator, "System", "Object", sizeof(Il2CppObject),
    il2cpp_utils::GetClassFromName("System.Collections", "IEnumerator"),
 
    DECLARE_CTOR(ctor, LightSwitchEventEffect *instance, BeatmapEventType eventType);
 
    DECLARE_INSTANCE_FIELD(Il2CppObject*, current);
    DECLARE_INSTANCE_FIELD(bool, hasWaited);
 
    DECLARE_INSTANCE_FIELD(LightSwitchEventEffect*, instance);
    DECLARE_INSTANCE_FIELD(BeatmapEventType, eventType);
 
    DECLARE_OVERRIDE_METHOD(bool, MoveNext, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "MoveNext"));
    DECLARE_OVERRIDE_METHOD(Il2CppObject*, get_Current, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "get_Current"));
    DECLARE_OVERRIDE_METHOD(void, Reset, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "Reset"));
 
    REGISTER_FUNCTION(WaitThenStartEnumerator,
        getLogger().debug("Registering WaitThenStartEnumerator!");
 
        REGISTER_FIELD(current);
        REGISTER_FIELD(hasWaited);
 
        REGISTER_FIELD(instance);
        REGISTER_FIELD(eventType);
 
        REGISTER_METHOD(ctor);
 
        REGISTER_METHOD(MoveNext);
        REGISTER_METHOD(get_Current);
        REGISTER_METHOD(Reset);
    )
)
 
DEFINE_CLASS(Il2CppNamespace::WaitThenStartEnumerator);
 
void Il2CppNamespace::WaitThenStartEnumerator::ctor(LightSwitchEventEffect *instance, BeatmapEventType eventType) {
    this->instance = instance;
    this->eventType = eventType;
    this->current = nullptr;
    this->hasWaited = false;
}
 
Il2CppObject* Il2CppNamespace::WaitThenStartEnumerator::get_Current() {
    return current;
}
 
void Il2CppNamespace::WaitThenStartEnumerator::Reset() {}
 
bool Il2CppNamespace::WaitThenStartEnumerator::MoveNext() {
    if (!hasWaited) {
        current = WaitForEndOfFrame::New_ctor();
        hasWaited = true;
        return true; // Continue coroutine
    }
 
    LightColorizer::LSEStart(instance, &eventType);
 
    return false; // Reached end of coroutine
}
 
IEnumerator *WaitThenStart(LightSwitchEventEffect *instance, BeatmapEventType eventType) {
    Il2CppNamespace::WaitThenStartEnumerator *coroutine = CRASH_UNLESS(il2cpp_utils::New<Il2CppNamespace::WaitThenStartEnumerator*>());
    return reinterpret_cast<IEnumerator*>(coroutine);
}

MAKE_HOOK_OFFSETLESS(LightSwitchEventEffect_Start, void, LightSwitchEventEffect* self) {
    self->StartCoroutine(WaitThenStart(self, self->event));

    LightSwitchEventEffect_Start(self);
}

void Chroma::Hooks::LightSwitchEventEffect() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), LightSwitchEventEffect_Start, il2cpp_utils::FindMethodUnsafe("", "LightSwitchEventEffect", "Start", 0));
}