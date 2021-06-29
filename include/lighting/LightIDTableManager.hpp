#pragma once

#include <unordered_map>
#include <optional>
#include <string>
#include <functional>

namespace Chroma {

    using EnvironmentLightDataT = std::unordered_map<int, std::unordered_map<int, int>>;

    class EnvironmentData {
        public:
            virtual EnvironmentLightDataT getEnvironmentLights() = 0;
    };

    using InstallEnvironmentFunc = std::pair<std::string, const EnvironmentLightDataT&>;

    class LightIDTableManager {
    private:
        inline static bool installed = false;
        inline static std::unordered_map<std::string, EnvironmentLightDataT> environmentsToInstall;

        inline static std::unordered_map<std::string, EnvironmentLightDataT> lightIdTable;

        inline static std::optional<EnvironmentLightDataT> activeTable = std::nullopt;

    public:
        LightIDTableManager() = delete;

        static std::optional<int> GetActiveTableValue(int type, int id);

        static void SetEnvironment(const std::string& environmentName);

        static void InitTable();

        static void RegisterIndex(int type, int index, std::optional<int> requestedKey);

        static void AddEnvironment(InstallEnvironmentFunc environmentData);
    };
}

#define ChromaInstallEnvironment(environment) \
struct __ChromaRegisterEnvironment##environment { \
    __ChromaRegisterEnvironment##environment() { \
        Chroma::LightIDTableManager::AddEnvironment({#environment, environment().getEnvironmentLights()}); \
    } \
}; \
static __ChromaRegisterEnvironment##environment __ChromaRegisterEnvironmentInstance##environment;