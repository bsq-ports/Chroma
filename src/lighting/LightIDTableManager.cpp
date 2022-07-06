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
        getLogger().info("Initializing environment data for %s", data.first.data());
        lightIdTable.emplace(data);
    }
    installed = true;
}

void LightIDTableManager::SetEnvironment(std::string_view environmentName) {
    auto it = lightIdTable.find(environmentName);

    if (it == lightIdTable.end()) {
        getLogger().warning("Table not found for %s", environmentName.data());
        activeTable = EnvironmentLightDataT();
        for (int i = 0; i < 10; i++){
            activeTable.value()[i] = {};
        }
    } else {
        getLogger().debug("Set the environment as %s", environmentName.data());
        auto map = it->second;
        activeTable = EnvironmentLightDataT(map.begin(), map.end());
    }
}

void LightIDTableManager::RegisterIndex(int lightId, int index, std::optional<int> requestedKey) {
    auto& table = activeTable.value();

    // To make fun of Aero, I'll keep the typo ;) https://github.com/Aeroluna/Chroma/commit/0f379e54e006de9dba0b64debcb64fb913b453cf#diff-efd8021f3aec91a9e88e1e6823f48c13605a7ef8b27790c9c3d4545860f43849R47
    auto& dictioanry = table[lightId];

    int key;

    if (requestedKey) {
        key = *requestedKey;
        while (dictioanry.contains(key))
            key++;
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
    getLogger().info("Registered key [%d] to type [%d]", key, lightId);
}

void LightIDTableManager::AddEnvironment(InstallEnvironmentFunc environmentData) {
    if (installed) {
        getLogger().info("Initializing environment data for %s", environmentData.first.data());
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
    if (it == activeTable->end()) return;

    auto& map = it->second;


    for (auto const [key, n] : map) {
        if (n == index) {
            map.erase(key);
            getLogger().info("Unregistered key [%d] from lightid [%d]", key, lightID);
            return;
        }
    }
}
