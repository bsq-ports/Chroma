#include "main.hpp"

#include "lighting/LightIDTableManager.hpp"

#include "lighting/environments/AllEnvironments.hpp"

using namespace Chroma;

void Chroma::LightIDTableManager::InitTable() {
    for (auto& data : environmentsToInstall) {
        getLogger().info("Initializing environment data for %s", data.first.c_str());
        lightIdTable.emplace(data);
    }
    installed = true;
}

void LightIDTableManager::SetEnvironment(const std::string& environmentName) {
    auto it = lightIdTable.find(environmentName);

    if (it == lightIdTable.end()) {
        getLogger().warning("Table not found for %s", environmentName.c_str());
        activeTable = std::nullopt;
    } else {
        getLogger().debug("Set the environment as %s", environmentName.c_str());
        auto map = it->second;
        activeTable = EnvironmentLightDataT(map.begin(), map.end());
    }
}

std::optional<int> LightIDTableManager::GetActiveTableValue(int type, int id) {
    if (activeTable)
    {
        auto& table = activeTable.value();

        auto typeTableIt = table.find(type);

        if (typeTableIt == table.end()) {
            getLogger().warning("Unable to find value for type %d", type);
            return std::nullopt;
        }

        const auto& typeTable = typeTableIt->second;
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

void LightIDTableManager::RegisterIndex(int type, int index, std::optional<int> requestedKey) {
    auto& table = activeTable.value();

    // To make fun of Aero, I'll keep the typo ;) https://github.com/Aeroluna/Chroma/commit/0f379e54e006de9dba0b64debcb64fb913b453cf#diff-efd8021f3aec91a9e88e1e6823f48c13605a7ef8b27790c9c3d4545860f43849R47
    auto& dictioanry = table[type];

    int key;

    if (requestedKey) {
        key = *requestedKey;
        while (table.find(key) != table.end())
            key++;
    } else {
        // TODO: Will this work?
        key = std::max_element(dictioanry.begin(), dictioanry.end())->first + 1;
    }


    dictioanry[key] = index;
    if (getChromaConfig().PrintEnvironmentEnhancementDebug.GetValue())
    {
        getLogger().info("Registered key [%d] to type [%d]", key, type);
    }
}

void LightIDTableManager::AddEnvironment(InstallEnvironmentFunc environmentData) {
    if (installed) {
        getLogger().info("Initializing environment data for %s", environmentData.first.c_str());
        lightIdTable.emplace(environmentData);
    } else {
        environmentsToInstall.emplace(environmentData);
    }
}
