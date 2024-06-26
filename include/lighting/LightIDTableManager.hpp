#pragma once

#include "main.hpp"
#include "ChromaLogger.hpp"

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

using InstallEnvironmentFunc = std::pair<std::string_view, EnvironmentLightDataT const&>;

class LightIDTableManager {
private:
  static bool installed;
  static std::unordered_map<std::string_view, EnvironmentLightDataT> environmentsToInstall;

  static std::unordered_map<std::string_view, EnvironmentLightDataT> lightIdTable;

  static std::optional<EnvironmentLightDataT> activeTable;

public:
  LightIDTableManager() = delete;

  static std::optional<int> GetActiveTableValue(int lightId, int id) {
    if (activeTable) {
      auto const& table = activeTable.value();

      auto typeTableIt = table.find(lightId);

      if (typeTableIt == table.end()) {
        ChromaLogger::Logger.warn("Unable to find value for type {}", lightId);
        return std::nullopt;
      }

      auto const& typeTable = typeTableIt->second;
      auto it = typeTable.find(id);

      if (it != typeTable.end()) {
        return it->second;
      } else {
        ChromaLogger::Logger.warn("Unable to find value for type {} and id {}.", lightId, id);
      }
    }

    ChromaLogger::Logger.warn("Return not found");
    return std::nullopt;
  }

  static std::optional<int> GetActiveTableValueReverse(int lightId, int id) {
    if (!activeTable) return std::nullopt;
    auto const& table = activeTable.value();

    auto typeTableIt = table.find(lightId);

    if (typeTableIt == table.end()) {
      return std::nullopt;
    }

    auto const& typeTable = typeTableIt->second;
    for (auto const& [key, value] : typeTable) {
      if (value == id) return key;
    }

    return std::nullopt;
  }

  static void SetEnvironment(std::string_view environmentName);

  static void InitTable();

  static void RegisterIndex(int lightId, int index, std::optional<int> requestedKey);
  static void UnregisterIndex(int lightID, int index);

  static void AddEnvironment(InstallEnvironmentFunc environmentData);
};
} // namespace Chroma

#define ChromaInstallEnvironment(environment)                                                                          \
  struct __ChromaRegisterEnvironment##environment {                                                                    \
    __ChromaRegisterEnvironment##environment() {                                                                       \
      ChromaLogger::Logger.info("Creating environment {}", #environment);                                              \
                                                                                                                       \
      Chroma::LightIDTableManager::AddEnvironment({ #environment, environment().getEnvironmentLights() });             \
    }                                                                                                                  \
  };                                                                                                                   \
  static __ChromaRegisterEnvironment##environment __ChromaRegisterEnvironmentInstance##environment;