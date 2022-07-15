#pragma once

#include <vector>
#include <optional>

#include "UnityEngine/Vector3.hpp"

#include "GlobalNamespace/TrackLaneRing.hpp"
#include "GlobalNamespace/BeatmapObjectsAvoidance.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "environment_enhancements/GameObjectInfo.hpp"

#include "sombrero/shared/Vector3Utils.hpp"
#include "sombrero/shared/QuaternionUtils.hpp"
#include "GeometryFactory.hpp"

namespace Chroma {
    enum class LookupMethod
    {
        Regex,
        Exact,
        Contains,
        StartsWith,
        EndsWith
    };

    struct ProfileData {
        std::chrono::high_resolution_clock::time_point time;
        std::string log;
        bool showTime = true;

        ProfileData(const std::chrono::high_resolution_clock::time_point &time, std::string_view log, bool showTime)
                : time(time), log(log), showTime(showTime) {}

        ProfileData(const std::chrono::high_resolution_clock::time_point &time, std::string_view log): time(time), log(log) {}

    };

    class Profiler {
    public:
        void startTimer() {
            start = std::chrono::high_resolution_clock::now();
        }

        void endTimer() {
            end = std::chrono::high_resolution_clock::now();
        }

        void mark(std::string_view name, bool log = true) {
            points.emplace_back(std::chrono::high_resolution_clock::now(), name, log);
        }

        void mark(ProfileData const& profileData) {
            points.emplace_back(profileData);
        }

        void printMarks() const {
            auto before = start;

            for (auto const& point : points) {
                auto time = point.time;
                auto const& name = point.log;

                if (point.showTime) {
                    auto difference = time - before;
                    auto millisElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(difference).count();
                    getLogger().debug("%s took %lldms", name.c_str(), millisElapsed);
                    before = time;
                } else {
                    getLogger().debug("%s", name.c_str());
                }
            }

            auto endMark = end ? end.value() : std::chrono::high_resolution_clock::now();

            auto finishTime = std::chrono::duration_cast<std::chrono::milliseconds>(endMark - start).count();

            getLogger().debug("Finished! Took %lldms", finishTime);
        }

        [[nodiscard]] auto elapsedTimeSinceNow() const {
            return std::chrono::high_resolution_clock::now() - start;
        }

        [[nodiscard]] auto elapsedTime() const {
            return end.value() - start;
        }



    private:
        std::chrono::high_resolution_clock::time_point start;
        std::optional<std::chrono::high_resolution_clock::time_point> end;

        // pair instead of map to keep order
        std::vector<ProfileData> points;
    };

    class EnvironmentEnhancementManager {
    private:
        inline static std::vector<GameObjectInfo> _globalGameObjectInfos;

        static std::vector<ByRef<const GameObjectInfo>> LookupId(std::string_view id, LookupMethod lookupMethod);

        static void GetAllGameObjects();

        static void GetChildRecursive(UnityEngine::Transform* gameObject, std::vector<UnityEngine::Transform*>& children);

    public:
        inline static std::unordered_map<GlobalNamespace::TrackLaneRing*, Sombrero::FastQuaternion> RingRotationOffsets;
        inline static std::unordered_map<GlobalNamespace::BeatmapObjectsAvoidance*, Sombrero::FastVector3> AvoidancePosition;
        inline static std::unordered_map<GlobalNamespace::BeatmapObjectsAvoidance*, Sombrero::FastQuaternion> AvoidanceRotation;

        static void Init(CustomJSONData::CustomBeatmapData* customBeatmapData, float noteLinesDistance);


    };
}
