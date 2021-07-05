#pragma once
#include "main.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"

namespace Chroma {
    class Hooks {
    private:
        inline static std::vector<void (*)(Logger &logger)> installFuncs;
    public:
        static void AddInstallFunc(void (*installFunc)(Logger &logger)) {
            installFuncs.push_back(installFunc);
        }

        static void InstallHooks(Logger &logger) {
            for (auto installFunc : installFuncs) {
                installFunc(logger);
            }
        }
    };
}
#define ChromaInstallHooks(func) \
struct __ChromaRegister##func { \
    __ChromaRegister##func() { \
        Chroma::Hooks::AddInstallFunc(func); \
    } \
}; \
static __ChromaRegister##func __ChromaRegisterInstance##func;