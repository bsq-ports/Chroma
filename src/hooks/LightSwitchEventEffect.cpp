#include "Chroma.hpp"

#include "System/Collections/Generic/IEnumerator_1.hpp"
#include "colorizer/LightColorizer.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "LightSwitchEventEffect.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;


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