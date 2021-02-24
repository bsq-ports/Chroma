#pragma once

#include "System/Collections/Generic/IEnumerator_1.hpp"
#include "colorizer/LightColorizer.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "LightSwitchEventEffect.hpp"

inline static std::optional<std::vector<GlobalNamespace::ILightWithId *>> OverrideLightWithIdActivation = std::nullopt;

DECLARE_CLASS_INTERFACES(Il2CppNamespace, WaitThenStartEnumerator, "System", "Object", sizeof(Il2CppObject),
il2cpp_utils::GetClassFromName("System.Collections", "IEnumerator"),

    DECLARE_CTOR(ctor, GlobalNamespace::LightSwitchEventEffect *instance, GlobalNamespace::BeatmapEventType eventType);

    DECLARE_INSTANCE_FIELD(Il2CppObject*, current);
    DECLARE_INSTANCE_FIELD(bool, hasWaited);

    DECLARE_INSTANCE_FIELD(GlobalNamespace::LightSwitchEventEffect*, instance);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapEventType, eventType);

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
