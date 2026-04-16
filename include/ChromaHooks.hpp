#pragma once
#include <algorithm>

#include "main.hpp"

// needed for ChromaInstallHooks
#include "ChromaLogger.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"

namespace Chroma {
class Hooks {
private:
  inline static std::vector<std::pair<std::string, void (*)()>> installFuncs;

public:
  static void AddInstallFunc(std::string name, void (*installFunc)()) {
    installFuncs.emplace_back(std::move(name), installFunc);
  }

  static void InstallHooks() {
    std::ranges::sort(installFuncs,
              [](std::pair<std::string, void (*)()> const& a, std::pair<std::string, void (*)()> const& b) { return a.first < b.first; });
    for (const auto& installFunc : installFuncs) {
      installFunc.second();
    }
  }
};
} // namespace Chroma
#define ChromaInstallHooks(func)                                                                                                           \
  struct __ChromaRegister##func {                                                                                                          \
    __ChromaRegister##func() {                                                                                                             \
      Chroma::Hooks::AddInstallFunc(#func, func);                                                                                          \
      ChromaLogger::Logger.info("ChromaHooks Registered install func: " #func);                                                                  \
    }                                                                                                                                      \
  };                                                                                                                                       \
  static __ChromaRegister##func __ChromaRegisterInstance##func;