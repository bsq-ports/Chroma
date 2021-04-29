#include "Chroma.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/typedefs-array.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/LightPairRotationEventEffect.hpp"
#include "GlobalNamespace/ParticleSystemEventEffect.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Mathf.hpp"
#include "GlobalNamespace/LightWithIdManager.hpp"
#include "GlobalNamespace/LightWithIdMonoBehaviour.hpp"
#include "GlobalNamespace/LightWithIds.hpp"
#include "lighting/LightIDTableManager.hpp"

#include <unordered_map>

#include "colorizer/LightColorizer.hpp"
#include "GlobalNamespace/TrackLaneRingsManager.hpp"
#include "GlobalNamespace/TrackLaneRing.hpp"
#include "utils/ChromaUtils.hpp"



using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

using namespace Chroma;




DEFINE_TYPE(Chroma::LSEColorManager);

void LightColorizer::Reset(MonoBehaviour *behaviour) {
    LSEColorManager* manager = Chroma::LSEColorManager::GetLSEColorManager(behaviour);

    if (manager != nullptr) {
        manager->Reset();
    }
}

void LightColorizer::ResetAllLightingColors() {
    for (auto& manager : lseColorManagers) {
        manager.second->Reset();
    }
}

void LightColorizer::SetLightingColors(MonoBehaviour *monoBehaviour, std::optional<UnityEngine::Color> Color0, std::optional<UnityEngine::Color> Color1,
                                       std::optional<UnityEngine::Color> Color0Boost, std::optional<UnityEngine::Color> Color1Boost) {
    auto manager = Chroma::LSEColorManager::GetLSEColorManager(monoBehaviour);

    if (manager != nullptr) {
        manager->SetLightingColors(Color0, Color1, Color0Boost, Color1Boost);
    }
}

void LightColorizer::SetLightingColors(BeatmapEventType beatmapEventType, std::optional<UnityEngine::Color> Color0, std::optional<UnityEngine::Color> Color1,
                                       std::optional<UnityEngine::Color> Color0Boost, std::optional<UnityEngine::Color> Color1Boost) {
    auto manager = Chroma::LSEColorManager::GetLSEColorManager(beatmapEventType);

    for (auto& l : LSEColorManager::GetLSEColorManager(beatmapEventType)) {
        l->SetLightingColors(Color0, Color1, Color0Boost, Color1Boost);
    }
}

void LightColorizer::SetAllLightingColors(std::optional<UnityEngine::Color> Color0, std::optional<UnityEngine::Color> Color1,
                                          std::optional<UnityEngine::Color> Color0Boost, std::optional<UnityEngine::Color> Color1Boost) {
    for (auto& manager : lseColorManagers) {
        manager.second->SetLightingColors(Color0, Color1, Color0Boost, Color1Boost);
    }
}

void LightColorizer::SetActiveColors(BeatmapEventType lse) {
    for (auto& l : LSEColorManager::GetLSEColorManager(lse)) {
        l->SetActiveColors();
    }
}

void LightColorizer::SetAllActiveColors() {
    for (auto& lseColorManager : lseColorManagers)
    {
        lseColorManager.second->SetActiveColors();
    }
}

void LightColorizer::ClearLSEColorManagers() {
    lseColorManagers.clear();
}

void LightColorizer::SetLastValue(MonoBehaviour *monoBehaviour, int val) {
    auto manager = LSEColorManager::GetLSEColorManager(monoBehaviour);

    if (manager != nullptr) {
        manager->SetLastValue(val);
    }
}

std::unordered_map<int, ILightWithId *> LightColorizer::GetLights(LightSwitchEventEffect *lse) {
    auto manager = LSEColorManager::GetLSEColorManager(lse);

    if (manager != nullptr) {
        return manager->Lights;
    }

    return {};
}

std::unordered_map<int, std::vector<ILightWithId *>> LightColorizer::GetLightsPropagationGrouped(LightSwitchEventEffect *lse) {
    auto manager = LSEColorManager::GetLSEColorManager(lse);

    if (manager != nullptr) {
        return manager->LightsPropagationGrouped;
    }

    return {};

}

void LightColorizer::LSEStart(MonoBehaviour *monoBehaviour, BeatmapEventType beatmapEventType) {
    LSEColorManager::CreateLSEColorManager(monoBehaviour, beatmapEventType);
}

void LightColorizer::RegisterLight(UnityEngine::MonoBehaviour *lightWithId) {

    if (ASSIGNMENT_CHECK(classof(LightWithIdMonoBehaviour*), lightWithId->klass)) {
        auto monoBehaviour = reinterpret_cast<LightWithIdMonoBehaviour*>(lightWithId);

        auto lse = LSEColorManager::GetLSEColorManager((monoBehaviour->get_lightId() - 1));

        LSEColorManager * monomanager;

            for (auto l : lse)
                if(l) {
                    monomanager = l;
                    break;
                }


            if (!monomanager) {
                return;
            }


            LightIDTableManager::RegisterIndex(monoBehaviour->get_lightId() - 1, monomanager->Lights.size());

            monomanager->Lights[monomanager->Lights.size()] = reinterpret_cast<ILightWithId*>(monoBehaviour);

    } else if (ASSIGNMENT_CHECK(classof(LightWithIds*), lightWithId->klass)) {
        auto lightWithIds = reinterpret_cast<LightWithIds *>(lightWithId);


        auto lightsWithIdArray = lightWithIds->lightIntensityData;


        for (int i = 0; i < lightsWithIdArray->Length(); i++) {
            auto light = reinterpret_cast<ILightWithId *>(lightsWithIdArray->values[i]);
            auto manager = LSEColorManager::GetLSEColorManager((light->get_lightId() - 1)).front();

            LightIDTableManager::RegisterIndex(light->get_lightId() - 1, manager->Lights.size());
            manager->Lights[manager->Lights.size()] = light;
        }

    }
}


namespace Chroma {

    void LSEColorManager::ctor(MonoBehaviour *mono, BeatmapEventType type) {
//        INVOKE_CTOR(LSEColorManager);
        static auto contextLogger = getLogger().WithContext(ChromaLogger::LightColorizer);

        _lse = mono;
        _type = type;
        InitializeSOs(mono, "_lightColor0", _lightColor0, _lightColor0_Original, _mLightColor0);
        InitializeSOs(mono, "_highlightColor0", _lightColor0, _lightColor0_Original, _mHighlightColor0);
        InitializeSOs(mono, "_lightColor1", _lightColor1, _lightColor1_Original, _mLightColor1);
        InitializeSOs(mono, "_highlightColor1", _lightColor1, _lightColor1_Original, _mHighlightColor1);


        if (ASSIGNMENT_CHECK(classof(LightSwitchEventEffect *), mono->klass)) {
            auto *lse = reinterpret_cast<LightSwitchEventEffect *>(mono);

            InitializeSOs(mono, "_lightColor0Boost", _lightColor0Boost, _lightColor0Boost_Original, _mLightColor0Boost);
            InitializeSOs(mono, "_highlightColor0Boost", _lightColor0Boost, _lightColor0Boost_Original,
                          _mHighlightColor0Boost);
            InitializeSOs(mono, "_lightColor1Boost", _lightColor1Boost, _lightColor1Boost_Original, _mLightColor1Boost);
            InitializeSOs(mono, "_highlightColor1Boost", _lightColor1Boost, _lightColor1Boost_Original,
                          _mHighlightColor1Boost);
            _supportBoostColor = true;

            auto lightManager = lse->lightManager;
            System::Collections::Generic::List_1<GlobalNamespace::ILightWithId *> *lightList = lightManager->lights->values[lse->lightsID];
            Array<ILightWithId *> *lightArray = lightList->items;


            Lights = std::unordered_map<int, ILightWithId*>();

            for (int i = 0; i < lightArray->get_Length(); i++) {
                auto l = lightArray->values[i];
                if (l == nullptr) continue;

                debugSpamLog(contextLogger, "Adding light to list");
                Lights[i] = l;
            }

            // Keep track of order
            int index = 0;
            std::unordered_map<int, int> insertionOrder;

            std::unordered_map<int, std::vector<ILightWithId *>> lightsPreGroup;

            auto managers = UnityEngine::Object::FindObjectsOfType<TrackLaneRingsManager*>();

            for (auto& light : Lights) {
                if (!light.second) continue;

                auto object = reinterpret_cast<Il2CppObject *>(light.second);

                debugSpamLog(contextLogger, "Doing light %s", object->klass->name);
                if (ASSIGNMENT_CHECK(classof(MonoBehaviour *), object->klass)) {
                    auto monoBehaviour = reinterpret_cast<MonoBehaviour *>(object);
                    int z = UnityEngine::Mathf::RoundToInt(monoBehaviour->get_transform()->get_position().z);

                    auto ring = monoBehaviour->GetComponentInParent<TrackLaneRing*>();

                    if (ring) {

                        TrackLaneRingsManager* mngr = nullptr;
                        auto indexR = 0;

                        for (int i = 0; i < managers->Length(); i++) {
                            auto m = managers->values[i];

                            if (m) {
                                indexR = m->rings->IndexOf(ring);
                                if (indexR >= 0) {
                                    mngr = m;
                                    break;
                                }
                            }
                        }

                        if (mngr != nullptr)
                            z = 1000 + indexR;

                    }

                    debugSpamLog(contextLogger, "Grouping to %d", z);

                    std::vector<ILightWithId *> list;

                    auto it = lightsPreGroup.find(z);
                    // Not found
                    if (it == lightsPreGroup.end()) {
                        insertionOrder[index] = z;
                        index++;
                    } else list = it->second;

                    list.push_back(light.second);

                    lightsPreGroup.insert_or_assign(it, z, list);
                }
            }


            std::unordered_map<int, std::vector<ILightWithId *>> lightsPreGroupFinal;

            int i = 0;

            while (i <= index) {
                debugSpamLog(contextLogger, "Doing the final grouping, prop id %d", i);
                int z = insertionOrder[i];

                lightsPreGroupFinal[i] = lightsPreGroup[z];
                i++;
            }

            debugSpamLog(contextLogger, "Done grouping, size %d", lightsPreGroup.size());

            LightsPropagationGrouped = lightsPreGroupFinal;
        }
    }

    std::vector<LSEColorManager *> LSEColorManager::GetLSEColorManager(BeatmapEventType type) {
        std::vector<LSEColorManager *> colorManagers;

        for (auto& man : LightColorizer::lseColorManagers) {
            if (man.second->_type == type)
                colorManagers.push_back(man.second);
        }

        return colorManagers;
    }

    LSEColorManager *LSEColorManager::GetLSEColorManager(MonoBehaviour *m) {
        auto it = LightColorizer::lseColorManagers.find(m);

        if (it != LightColorizer::lseColorManagers.end()) return it->second;

        return nullptr;
    }

    LSEColorManager *LSEColorManager::CreateLSEColorManager(MonoBehaviour *lse, BeatmapEventType type) {
        LSEColorManager * lsecm = CRASH_UNLESS(il2cpp_utils::New<LSEColorManager *>(lse, type));
        LightColorizer::lseColorManagers[lse] = lsecm;
        return lsecm;
    }

    void LSEColorManager::Reset() const {
        _lightColor0->SetColor(_lightColor0_Original);
        _lightColor1->SetColor(_lightColor1_Original);
        if (_supportBoostColor) {
            _lightColor0Boost->SetColor(_lightColor0Boost_Original);
            _lightColor1Boost->SetColor(_lightColor1Boost_Original);
        }
    }

    void LSEColorManager::SetLightingColors(std::optional<UnityEngine::Color> Color0,
                                            std::optional<UnityEngine::Color> Color1,
                                            std::optional<UnityEngine::Color> Color0Boost,
                                            std::optional<UnityEngine::Color> Color1Boost) const {
        static auto contextLogger = getLogger().WithContext(ChromaLogger::LightColorizer);
        debugSpamLog(contextLogger, "_lightColor0 is null %s", _lightColor0 ? "false" : "true");
        if (Color0) {
            _lightColor0->SetColor(Color0.value());
        }

        debugSpamLog(contextLogger, "_lightColor1 is null %s", _lightColor1 ? "false" : "true");
        if (Color1) {
            _lightColor1->SetColor(Color1.value());
        }

        if (_supportBoostColor) {
            debugSpamLog(contextLogger, "_lightColor0Boost is null %s", _lightColor0Boost ? "false" : "true");
            if (Color0Boost) {
                _lightColor0Boost->SetColor(Color0Boost.value());
            }

            debugSpamLog(contextLogger, "_lightColor1Boost is null %s", _lightColor1Boost ? "false" : "true");
            if (Color1Boost) {
                _lightColor1Boost->SetColor(Color1Boost.value());
            }
        }
    }

    void LSEColorManager::SetLastValue(int value) {
        _lastValue = (float) value;
    }

    void LSEColorManager::SetActiveColors() const {
        // Replace with ProcessLightSwitchEvent
        if (_lastValue <= 0) {
            return;
        }

        bool warm;

        switch ((int) _lastValue) {
            case 1:
            case 2:
            case 3:
            default:
                warm = false;
                break;

            case 5:
            case 6:
            case 7:
                warm = true;
                break;
        }

        UnityEngine::Color c;
        switch ((int) _lastValue) {
            case 1:
            case 5:
            default:
                c = warm ? _mLightColor0->get_color() : _mLightColor1->get_color();
                break;

            case 2:
            case 6:
            case 3:
            case 7:
                c = warm ? _mHighlightColor0->get_color() : _mHighlightColor1->get_color();
                break;
        }

        if (_lse->get_enabled()) {
            if (ASSIGNMENT_CHECK(classof(LightSwitchEventEffect *), _lse->klass )) {
                auto l1 = reinterpret_cast<LightSwitchEventEffect *>(_lse);
                l1->highlightColor = c;
            } else if (ASSIGNMENT_CHECK(classof(ParticleSystemEventEffect *), _lse->klass )) {
                auto p1 = reinterpret_cast<ParticleSystemEventEffect *>(_lse);

                p1->highlightColor = c;
            }

            if (_lastValue == 3 || _lastValue == 7) {
                if (ASSIGNMENT_CHECK(classof(LightSwitchEventEffect *), _lse->klass)) {
                    auto l1 = reinterpret_cast<LightSwitchEventEffect *>(_lse);
                    c.a = 0.0f;
                    l1->afterHighlightColor = c;
                } else if (ASSIGNMENT_CHECK(classof(ParticleSystemEventEffect *), _lse->klass)) {
                    auto p1 = reinterpret_cast<ParticleSystemEventEffect *>(_lse);
                    c.a = 0.0f;
                    p1->afterHighlightColor = c;
                }
            } else {
                if (ASSIGNMENT_CHECK(classof(LightSwitchEventEffect *), _lse->klass)) {
                    auto l1 = reinterpret_cast<LightSwitchEventEffect *>(_lse);
                    l1->afterHighlightColor = c;
                } else if (ASSIGNMENT_CHECK(classof(ParticleSystemEventEffect *), _lse->klass)) {
                    auto p1 = reinterpret_cast<ParticleSystemEventEffect *>(_lse);

                    p1->afterHighlightColor = c;
                }
            }
        } else {
            if (_lastValue == 1 || _lastValue == 5 || _lastValue == 2 || _lastValue == 6) {
                if (ASSIGNMENT_CHECK(classof(LightSwitchEventEffect *), _lse->klass)) {
                    auto l1 = reinterpret_cast<LightSwitchEventEffect *>(_lse);
                    l1->SetColor(c);
                } else if (ASSIGNMENT_CHECK(classof(ParticleSystemEventEffect *), _lse->klass)) {
                    auto p1 = reinterpret_cast<ParticleSystemEventEffect *>(_lse);

                    p1->particleColor = c;
                    p1->RefreshParticles();
                }
            }
        }

        if (ASSIGNMENT_CHECK(classof(LightSwitchEventEffect *), _lse->klass)) {
            auto l1 = reinterpret_cast<LightSwitchEventEffect *>(_lse);
            c.a = 0.0f;
            l1->offColor = c;
        } else if (ASSIGNMENT_CHECK(classof(ParticleSystemEventEffect *), _lse->klass)) {
            auto p1 = reinterpret_cast<ParticleSystemEventEffect *>(_lse);
            c.a = 0.0f;
            p1->offColor = c;
        }
    }


    void LSEColorManager::InitializeSOs(MonoBehaviour *lse, const std::string &id, SimpleColorSO *&sColorSO,
                                        UnityEngine::Color &originalColor, MultipliedColorSO *&mColorSO) {

        // Todo: Use codegen here
        MultipliedColorSO *lightMultSO = nullptr;
        if (ASSIGNMENT_CHECK(classof(LightSwitchEventEffect *), lse->klass)) {
            auto l1 = reinterpret_cast<LightSwitchEventEffect *>(lse);
            lightMultSO = reinterpret_cast<MultipliedColorSO *>(il2cpp_utils::GetFieldValue<ColorSO *>(l1,
                                                                                                       id).value()); //l1.GetField<ColorSO, LightSwitchEventEffect>(id);
        } else if (ASSIGNMENT_CHECK(classof(ParticleSystemEventEffect *), lse->klass)) {
            auto p1 = reinterpret_cast<ParticleSystemEventEffect *>(lse);
            lightMultSO = reinterpret_cast<MultipliedColorSO *>(il2cpp_utils::GetFieldValue<ColorSO *>(p1,
                                                                                                       id).value()); //l1.GetField<ColorSO, LightSwitchEventEffect>(id);
        }

        UnityEngine::Color multiplierColor = lightMultSO->multiplierColor;
        SimpleColorSO *lightSO = lightMultSO->baseColor;
        originalColor = lightSO->color;

        if (mColorSO == nullptr) {
            mColorSO = ScriptableObject::CreateInstance<MultipliedColorSO *>();
            mColorSO->multiplierColor = multiplierColor; //.SetField("_multiplierColor", multiplierColor);

            if (sColorSO == nullptr) {
                sColorSO = ScriptableObject::CreateInstance<SimpleColorSO *>();
                sColorSO->SetColor(originalColor);
            }

            mColorSO->baseColor = sColorSO; //SetField("_baseColor", sColorSO);
        }

        static auto contextLogger = getLogger().WithContext(ChromaLogger::LightColorizer);
        debugSpamLog(contextLogger, "Overwriting %s", id.c_str());
        if (ASSIGNMENT_CHECK(classof(LightSwitchEventEffect *), lse->klass)) {
            CRASH_UNLESS(mColorSO);
            auto l1 = reinterpret_cast<LightSwitchEventEffect *>(lse);

            il2cpp_utils::SetFieldValue<LightSwitchEventEffect *, ColorSO *>(l1, id,
                                                                             mColorSO); //l1.GetField<ColorSO, LightSwitchEventEffect>(id);
        } else if (ASSIGNMENT_CHECK(classof(ParticleSystemEventEffect *), lse->klass)) {
            CRASH_UNLESS(mColorSO);
            auto p1 = reinterpret_cast<ParticleSystemEventEffect *>(lse);

            il2cpp_utils::SetFieldValue<ParticleSystemEventEffect *, ColorSO *>(p1, id,
                                                                                mColorSO); //l1.GetField<ColorSO, LightSwitchEventEffect>(id);
        }
    }
}