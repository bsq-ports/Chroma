#include "main.hpp"

#include "lighting/LightIDTableManager.hpp"

#include "lighting/environments/AllEnvironments.hpp"

using namespace Chroma;

bool LightIDTableManager::installed = false;
std::unordered_map<std::string_view, EnvironmentLightDataT> LightIDTableManager::environmentsToInstall;

std::unordered_map<std::string_view, EnvironmentLightDataT> LightIDTableManager::lightIdTable;
std::optional<EnvironmentLightDataT> LightIDTableManager::activeTable = std::nullopt;

void Chroma::LightIDTableManager::InitTable() {
  for (auto& data : environmentsToInstall) {
    ChromaLogger::Logger.info("Initializing environment data for {}", data.first.data());
    lightIdTable.emplace(data);
  }
  installed = true;
}

void LightIDTableManager::SetEnvironment(std::string_view environmentName) {
  auto it = lightIdTable.find(environmentName);

  if (it == lightIdTable.end()) {
    ChromaLogger::Logger.warn("Table not found for {}", environmentName.data());
    activeTable = EnvironmentLightDataT();
    for (int i = 0; i < 10; i++) {
      activeTable.value()[i] = {};
    }
  } else {
    ChromaLogger::Logger.debug("Set the environment as {}", environmentName.data());
    auto map = it->second;
    activeTable = EnvironmentLightDataT(map.begin(), map.end());
  }
}

void LightIDTableManager::RegisterIndex(int lightId, int index, std::optional<int> requestedKey) {
  auto& table = activeTable.value();

  // To make fun of Aero, I'll keep the typo ;)
  // https://github.com/Aeroluna/Chroma/commit/0f379e54e006de9dba0b64debcb64fb913b453cf#diff-efd8021f3aec91a9e88e1e6823f48c13605a7ef8b27790c9c3d4545860f43849R47
  auto& dictioanry = table[lightId];

  int key = 0;

  if (requestedKey) {
    key = *requestedKey;
    while (dictioanry.contains(key)) {
      key++;
    }
  } else {
    if (dictioanry.empty()) {
      key = 0;
    } else {
      for (auto const& [k, _] : dictioanry) {
        key = std::max(key, k);
      }

      key++;
    }
  }

  dictioanry[key] = index;
  ChromaLogger::Logger.info("Registered key [{}] to type [{}]", key, lightId);
}

void LightIDTableManager::AddEnvironment(InstallEnvironmentFunc environmentData) {
  if (installed) {
    ChromaLogger::Logger.info("Initializing environment data for {}", environmentData.first.data());
    lightIdTable.try_emplace(environmentData.first, environmentData.second);
  } else {
    environmentsToInstall.try_emplace(environmentData.first, environmentData.second);
  }
}

void LightIDTableManager::UnregisterIndex(int lightID, int index) {
  if (!activeTable) {
    return;
  }

  auto it = activeTable->find(lightID);
  if (it == activeTable->end()) {
    return;
  }

  auto& map = it->second;

  for (auto const [key, n] : map) {
    if (n != index) {
      continue;
    }

    map.erase(key);
    ChromaLogger::Logger.info("Unregistered key [{}] from lightid [{}]", key, lightID);
    return;
  }
}
std::optional<int> Chroma::LightIDTableManager::GetActiveTableValueReverse(int lightId, int id) {
  if (!activeTable) {
    return std::nullopt;
  }
  auto const& table = activeTable.value();

  auto typeTableIt = table.find(lightId);

  if (typeTableIt == table.end()) {
    return std::nullopt;
  }

  auto const& typeTable = typeTableIt->second;
  for (auto const& [key, value] : typeTable) {
    if (value == id) {
      return key;
    }
  }

  return std::nullopt;
}
std::optional<int> Chroma::LightIDTableManager::GetActiveTableValue(int lightId, int id) {
  if (!activeTable) {

    ChromaLogger::Logger.warn("Return not found");
    return std::nullopt;
  }
  auto const& table = activeTable.value();

  auto typeTableIt = table.find(lightId);

  if (typeTableIt == table.end()) {
    ChromaLogger::Logger.warn("Unable to find value for type {}", lightId);
    return std::nullopt;
  }

  auto const& typeTable = typeTableIt->second;
  auto it = typeTable.find(id);

  if (it == typeTable.end()) {
    ChromaLogger::Logger.warn("Unable to find value for type {} and id {}.", lightId, id);

    return std::nullopt;
  }

  return it->second;
}
