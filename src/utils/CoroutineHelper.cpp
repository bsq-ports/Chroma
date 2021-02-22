#include "utils/CoroutineHelper.hpp"

DEFINE_CLASS(Chroma::CoroutineRunner)

using namespace CustomJSONData;
using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;


bool CoroutineRunner::MoveNext() {
    coroutine++;
}

void CoroutineRunner::Reset() {
    coroutine = coroutine.begin();
}

Il2CppObject * CoroutineRunner::get_Current() {
    return instance;
}

CoroutineRunner::CoroutineRunner(UnityEngine::MonoBehaviour *monobehaviour,
                                 std::experimental::generator<void *> coroutine) {
    instance = monobehaviour;
    this->coroutine = coroutine;
}