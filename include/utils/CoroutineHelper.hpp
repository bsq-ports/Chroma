#pragma once

#include <functional>
#include "System/Collections/Generic/IEnumerator_1.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"


#include <experimental/coroutine>
#include <type_traits>
#include <utility>
#include <exception>
#include <iterator>
#include <functional>
#include "generator.hpp"
#include <memory>


#include "UnityEngine/MonoBehaviour.hpp"

#include "main.hpp"


DECLARE_CLASS_INTERFACES(Chroma, CoroutineRunner, "System", "Object", sizeof(Il2CppObject),
                         il2cpp_utils::GetClassFromName("System.Collections", "IEnumerator"),

         public:
                //
                typedef std::generator<const void*> coroutineType;

                 std::generator<const void*>& coroutine;
                 std::optional<std::generator<const void*>::iterator>& coroutineIterator;

                 static CoroutineRunner* Create(coroutineType coroutineType1);

                 DECLARE_CTOR(ctor, void* corountine );
                 DECLARE_OVERRIDE_METHOD(void, Reset, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "Reset"));


//                 DECLARE_OVERRIDE_METHOD(void, Finalize,il2cpp_utils::FindMethod("System", "Object", "Finalize"));
                    DECLARE_OVERRIDE_METHOD(Il2CppObject*, get_Current, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "get_Current"));

                 DECLARE_OVERRIDE_METHOD(bool, MoveNext, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "MoveNext"));
//                                 DECLARE_OVERRIDE_METHOD(void, Reset, il2cpp_utils::FindMethod("System.Collections", "IEnumerator", "Reset"));

                REGISTER_FUNCTION(CoroutineRunner,
        getLogger().debug("Registering CoroutineHelper!");

                REGISTER_METHOD(ctor);
                REGISTER_METHOD(MoveNext);
                REGISTER_METHOD(get_Current);
//        REGISTER_METHOD(Finalize);
                 REGISTER_METHOD(Reset);
        )
)