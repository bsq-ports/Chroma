#pragma once

#include <functional>
#include "System/Collections/Generic/IEnumerator_1.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include <coroutine>

#include "UnityEngine/MonoBehaviour.hpp"

namespace Chroma {
//    template<typename ReturnType, typename... Parameters>
//    class Coroutine {
//    public:
//        typedef std::function<ReturnType, (bool&, Parameters)> CoroutineFunction;
//
//    private:
//        inline bool hasWaited = false;
//        inline CoroutineFunction coroutineFunc;
//        inline bool done = false;
//
//    public:
//        Coroutine(CoroutineFunction coroutineFunction);
//
//        void Start(Parameters... parameters) {
//            if (!done) {
//                coroutineFunc(done, parameters);
//            }
//        }
//
//    };



}


DECLARE_CLASS_INTERFACES(Chroma, CoroutineRunner, "System", "Object", sizeof(Il2CppObject),
                         il2cpp_utils::GetClassFromName("System.Collections", "IEnumerator"),

                            public:
                                 CoroutineRunner(UnityEngine::Monobehaviour* monobehaviour, std::experimental::generator<void*> coroutine);
                                 std::experimental::generator<void*> coroutine;

                                 DECLARE_INSTANCE_FIELD(bool, frameNext);

                                 DECLARE_INSTANCE_FIELD(UnityEngine::Monobehaviour*, instance);

                                 DECLARE_OVERRIDE_METHOD(bool, MoveNext, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "MoveNext"));
                                 DECLARE_OVERRIDE_METHOD(Il2CppObject*, get_Current, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "get_Current"));
                                 DECLARE_OVERRIDE_METHOD(void, Reset, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "Reset"));

                                 REGISTER_FUNCTION(WaitThenStartEnumerator,
                                                   getLogger().debug("Registering CoroutineHelper!");


                                 REGISTER_FIELD(instance);
                                 REGISTER_FIELD(frameNext);

                                 REGISTER_METHOD(MoveNext);
                                 REGISTER_METHOD(get_Current);
                                 REGISTER_METHOD(Reset);
                         )
)