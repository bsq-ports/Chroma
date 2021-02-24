#include "utils/CoroutineHelper.hpp"
#include <cassert>
#include <utility>
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"

DEFINE_CLASS(Chroma::CoroutineRunner);

using namespace Chroma;
using namespace UnityEngine;
using namespace System::Collections;


bool CoroutineRunner::MoveNext() {
    if (!coroutineIterator) coroutineIterator = std::make_optional(coroutine.begin());
    else coroutineIterator.value()++;


    return coroutineIterator.value() != coroutine.end();
}

Il2CppObject * CoroutineRunner::get_Current() {
    return (Il2CppObject*) nullptr;
}

//void CoroutineRunner::Finalize() {
//    delete this;
//}


// This code was kindly given and documented by raftario. Thank you so much for dedicating time and effort into helping
CoroutineRunner* CoroutineRunner::Create(coroutineType coroutine)
{
    // We allocate enough memory on the heap to store the coroutine with `malloc`,
    // which returns a pointer to that memory
    // This is necessary because C# doesn't know about `std::generator`, but it knows about pointers
    // We could not use a reference to the coroutine (`&coroutine`) because once this function returns,
    // `coroutine` will not exist anymore as it lives on the stack
    // TL;DR - stack values only live as long as the scope they are declared it,
    // while heap values live forever or until they are explicitely freed
    // `unique_ptr` does this implicitely when it is created
    coroutineType *ptr = ptr = (coroutineType *) malloc(sizeof(coroutineType));

    // We write the coroutine to the memory location we just allocated
    // We now have two copies of it; one on the stack and one of the heap
    *ptr = std::move(coroutine);

    // Pass a pointer to the copy that lives on the heap
    CoroutineRunner* coroutineRunner = CRASH_UNLESS(il2cpp_utils::New<Chroma::CoroutineRunner*>((void*) ptr));

    return coroutineRunner;

    // The copy that lives on the stack stops existing here
}

void CoroutineRunner::Reset() {
    coroutine.end();
    coroutineIterator = std::nullopt;
}

void CoroutineRunner::ctor(void *ptr)
{
    // We dereference the pointer to our generator, creating a new copy on the stack
    auto generator = *(coroutineType *)ptr;

    // We move the copy living on the stack to `this->coroutine`
    this->coroutine = std::move(generator);

    // Since we no longer need the copy that lives on the heap, we free the memory it uses, making it available again
    // Not freeing heap values is what causes memory leaks
    // `unique_ptr` does this implicitely when it goes out of scope
    free(ptr);

    // The copy living on the stack dies here, but that's fine since we moved it
}