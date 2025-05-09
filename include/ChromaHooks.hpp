#pragma once
#include "main.hpp"

// needed for ChromaInstallHooks
#include "ChromaLogger.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"

namespace Chroma {
class Hooks {
private:
  inline static std::vector<void (*)()> installFuncs;

public:
  static void AddInstallFunc(void (*installFunc)()) {
    installFuncs.push_back(installFunc);
  }

  static void InstallHooks() {
    for (auto installFunc : installFuncs) {
      installFunc();
    }
  }
};
} // namespace Chroma
#define ChromaInstallHooks(func)                                                                                       \
  struct __ChromaRegister##func {                                                                                      \
    __ChromaRegister##func() {                                                                                         \
      Chroma::Hooks::AddInstallFunc(func);                                                                             \
    }                                                                                                                  \
  };                                                                                                                   \
  static __ChromaRegister##func __ChromaRegisterInstance##func;