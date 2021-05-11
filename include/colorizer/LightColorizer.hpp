#pragma once

#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/ILightWithId.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include "GlobalNamespace/LightWithIdManager.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include <vector>
#include <string>
#include <optional>
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "main.hpp"

// TODO: Document properly
namespace Chroma {
    class LSEColorManager;

    class LightColorizer {
    public:
        static void Reset(UnityEngine::MonoBehaviour *behaviour);

        static void ResetAllLightingColors();

        static void RegisterLight(UnityEngine::MonoBehaviour* lightWithId);

        static void
        SetLightingColors(UnityEngine::MonoBehaviour *monoBehaviour, std::optional<UnityEngine::Color> Color0,
                          std::optional<UnityEngine::Color> Color1,
                          std::optional<UnityEngine::Color> Color0Boost = std::nullopt,
                          std::optional<UnityEngine::Color> Color1Boost = std::nullopt);

        static void
        SetLightingColors(GlobalNamespace::BeatmapEventType beatmapEventType, std::optional<UnityEngine::Color> Color0,
                          std::optional<UnityEngine::Color> Color1,
                          std::optional<UnityEngine::Color> Color0Boost = std::nullopt,
                          std::optional<UnityEngine::Color> Color1Boost = std::nullopt);

        static void
        SetAllLightingColors(std::optional<UnityEngine::Color> Color0, std::optional<UnityEngine::Color> Color1,
                             std::optional<UnityEngine::Color> Color0Boost = std::nullopt,
                             std::optional<UnityEngine::Color> Color1Boost = std::nullopt);

        static void SetActiveColors(GlobalNamespace::BeatmapEventType lse);

        static void SetAllActiveColors();

        static void ClearLSEColorManagers();

        static void SetLastValue(UnityEngine::MonoBehaviour *monoBehaviour, int val);

        // Returns array of ILightWithId*
        static std::unordered_map<int, GlobalNamespace::ILightWithId *> GetLights(GlobalNamespace::LightSwitchEventEffect *lse);

        // Returns 2d array of ILightWithId*
        static std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>>
        GetLightsPropagationGrouped(GlobalNamespace::LightSwitchEventEffect *lse);

        static void
        LSEStart(UnityEngine::MonoBehaviour *monoBehaviour, GlobalNamespace::BeatmapEventType beatmapEventType);

        inline static std::unordered_map<UnityEngine::MonoBehaviour*, Chroma::LSEColorManager *> lseColorManagers;
    };
}

typedef std::unordered_map<int, std::vector<GlobalNamespace::ILightWithId *>> LightGroupMap;

// We use custom types here to avoid GC deleting our variables
DECLARE_CLASS_CODEGEN(Chroma, LSEColorManager, Il2CppObject,

        public:


            std::unordered_map<int, GlobalNamespace::ILightWithId*> Lights = {};

            // 2d array of ILightWithId*
            LightGroupMap LightsPropagationGrouped;

            // TODO: Is this the proper return type?
            static std::vector<LSEColorManager *> GetLSEColorManager(GlobalNamespace::BeatmapEventType type);

            static LSEColorManager *GetLSEColorManager(UnityEngine::MonoBehaviour *m);

            static LSEColorManager *CreateLSEColorManager(UnityEngine::MonoBehaviour *lse, GlobalNamespace::BeatmapEventType type);

            void Reset() const;


            void SetLightingColors(std::optional<UnityEngine::Color> Color0, std::optional<UnityEngine::Color> Color1,
            std::optional<UnityEngine::Color> Color0Boost = std::nullopt, std::optional<UnityEngine::Color> Color1Boost = std::nullopt) const;

            void SetLastValue(int value);

            void SetActiveColors() const;

            void InitializeSOs(UnityEngine::MonoBehaviour *lse, const std::string& id, GlobalNamespace::SimpleColorSO *&sColorSO,
            UnityEngine::Color& originalColor, GlobalNamespace::MultipliedColorSO *&mColorSO);

            DECLARE_INSTANCE_FIELD(UnityEngine::MonoBehaviour *, _lse);
            DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapEventType, _type);

            DECLARE_INSTANCE_FIELD(UnityEngine::Color, _lightColor0_Original);
            DECLARE_INSTANCE_FIELD(UnityEngine::Color, _lightColor1_Original);
            DECLARE_INSTANCE_FIELD(UnityEngine::Color, _lightColor0Boost_Original);
            DECLARE_INSTANCE_FIELD(UnityEngine::Color, _lightColor1Boost_Original);

            DECLARE_INSTANCE_FIELD(GlobalNamespace::SimpleColorSO*, _lightColor0);
            DECLARE_INSTANCE_FIELD(GlobalNamespace::SimpleColorSO*, _lightColor1);
            DECLARE_INSTANCE_FIELD(GlobalNamespace::SimpleColorSO*, _lightColor0Boost);
            DECLARE_INSTANCE_FIELD(GlobalNamespace::SimpleColorSO*, _lightColor1Boost);

            DECLARE_INSTANCE_FIELD(GlobalNamespace::MultipliedColorSO*, _mLightColor0);
            DECLARE_INSTANCE_FIELD(GlobalNamespace::MultipliedColorSO*, _mHighlightColor0);
            DECLARE_INSTANCE_FIELD(GlobalNamespace::MultipliedColorSO*, _mLightColor1);
            DECLARE_INSTANCE_FIELD(GlobalNamespace::MultipliedColorSO*, _mHighlightColor1);

            DECLARE_INSTANCE_FIELD(GlobalNamespace::MultipliedColorSO*, _mLightColor0Boost);
            DECLARE_INSTANCE_FIELD(GlobalNamespace::MultipliedColorSO*, _mHighlightColor0Boost);
            DECLARE_INSTANCE_FIELD(GlobalNamespace::MultipliedColorSO*, _mLightColor1Boost);
            DECLARE_INSTANCE_FIELD(GlobalNamespace::MultipliedColorSO*, _mHighlightColor1Boost);

            DECLARE_INSTANCE_FIELD(bool, _supportBoostColor);

            DECLARE_INSTANCE_FIELD(float, _lastValue);

            DECLARE_SIMPLE_DTOR();
            DECLARE_CTOR(ctor, UnityEngine::MonoBehaviour* mono, GlobalNamespace::BeatmapEventType type);

            REGISTER_FUNCTION(
                getLogger().debug("Registering LSEColorManager!");
                REGISTER_METHOD(ctor);
                REGISTER_SIMPLE_DTOR();

                REGISTER_FIELD(_lse);
                REGISTER_FIELD(_type);

                REGISTER_FIELD(_lightColor0_Original);
                REGISTER_FIELD(_lightColor1_Original);
                REGISTER_FIELD(_lightColor0Boost_Original);
                REGISTER_FIELD(_lightColor1Boost_Original);

                REGISTER_FIELD(_lightColor0);
                REGISTER_FIELD(_lightColor1);
                REGISTER_FIELD(_lightColor0Boost);
                REGISTER_FIELD(_lightColor1Boost);

                REGISTER_FIELD(_mLightColor0);
                REGISTER_FIELD(_mHighlightColor0);
                REGISTER_FIELD(_mLightColor1);
                REGISTER_FIELD(_mHighlightColor1);

                REGISTER_FIELD(_mLightColor0Boost);
                REGISTER_FIELD(_mHighlightColor0Boost);
                REGISTER_FIELD(_mLightColor1Boost);
                REGISTER_FIELD(_mHighlightColor1Boost);
            )
)


