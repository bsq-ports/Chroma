#pragma once

#include <unordered_map>
#include <optional>
#include <string>

namespace Chroma {

    typedef std::unordered_map<int, std::unordered_map<int, int>> EnvironmentLightDataT;

    class EnvironmentData {
        public:
            virtual EnvironmentLightDataT getEnvironmentLights() = 0;
    };


    class LightIDTableManager {
    private:
        inline static std::unordered_map<std::string, EnvironmentLightDataT> lightIdTable;

        inline static std::optional<EnvironmentLightDataT> activeTable = std::nullopt;

    public:
        LightIDTableManager() = delete;

        static std::optional<int> GetActiveTableValue(int type, int id);

        static void SetEnvironment(const std::string& environmentName);

        static void InitTable();

        static void RegisterIndex(int type, int index);
    };

}