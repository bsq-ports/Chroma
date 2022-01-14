#pragma once

#include "main.hpp"

#include <unordered_map>
#include <optional>
#include <string>
#include <functional>

namespace Chroma {

    using EnvironmentLightDataT = std::unordered_map<int, std::unordered_map<int, int>>;

    class EnvironmentData {
        public:
            virtual constexpr EnvironmentLightDataT getEnvironmentLights() = 0;
    };

    using InstallEnvironmentFunc = std::pair<std::string_view, const EnvironmentLightDataT&>;

    class LightIDTableManager {
    private:
        static bool installed;
        static std::unordered_map<std::string_view, EnvironmentLightDataT> environmentsToInstall;

        static std::unordered_map<std::string_view, EnvironmentLightDataT> lightIdTable;

        static std::optional<EnvironmentLightDataT> activeTable;

    public:
        LightIDTableManager() = delete;

        static std::optional<int> GetActiveTableValue(int type, int id) {
            if (activeTable) {
                auto const& table = activeTable.value();

                auto typeTableIt = table.find(type);

                if (typeTableIt == table.end()) {
                    getLogger().warning("Unable to find value for type %d", type);
                    return std::nullopt;
                }

                const auto &typeTable = typeTableIt->second;
                auto it = typeTable.find(id);

                if (it != typeTable.end()) {
                    return it->second;
                } else {
                    getLogger().warning("Unable to find value for type %d and id %d.", type, id);
                }
            }

            getLogger().warning("Return not found");
            return std::nullopt;
        }

        static std::optional<int> GetActiveTableValueReverse(int type, int id) {
            if (activeTable) {
                auto const& table = activeTable.value();

                auto typeTableIt = table.find(type);

                if (typeTableIt == table.end()) {
                    return std::nullopt;
                }

                const auto &typeTable = typeTableIt->second;
                for (auto const& [key, value] : typeTable) {
                    if (value == id)
                        return key;
                }
            }

            return std::nullopt;
        }

        static void SetEnvironment(const std::string& environmentName);

        static void InitTable();

        static void RegisterIndex(int type, int index, std::optional<int> requestedKey);

        static void AddEnvironment(InstallEnvironmentFunc environmentData);
    };
}

#define ChromaInstallEnvironment(environment) \
struct __ChromaRegisterEnvironment##environment { \
    __ChromaRegisterEnvironment##environment() {  \
        __android_log_print(Logging::INFO, "QuestHook[Chroma]", "Creating environment " #environment);\
                                              \
        Chroma::LightIDTableManager::AddEnvironment({#environment, environment().getEnvironmentLights()}); \
    } \
}; \
static __ChromaRegisterEnvironment##environment __ChromaRegisterEnvironmentInstance##environment;