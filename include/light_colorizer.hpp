#pragma once

#include <map>
#include <unordered_set>
#include <vector>

#include "GlobalNamespace/BeatmapEventType.hpp"
#include "GlobalNamespace/ILightWithId.hpp"
#include "GlobalNamespace/LightSwitchEventEffect.hpp"
#include "GlobalNamespace/LightWithIdManager.hpp"
#include "GlobalNamespace/MultipliedColorSO.hpp"
#include "GlobalNamespace/ParticleSystemEventEffect.hpp"
#include "GlobalNamespace/SimpleColorSO.hpp"
#include "System/Collections/Generic/List_1.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"

using namespace std;

using namespace GlobalNamespace;
using namespace System::Collections::Generic;
using namespace UnityEngine;

class LightColorizer {
private:
    class LSEColorManager;

    static unordered_set<LSEColorManager*> lseColorManagers;

    class LSEColorManager {
    private:
        MonoBehaviour* lse;
        BeatmapEventType type;

        UnityEngine::Color lightColor0_Original;
        UnityEngine::Color lightColor1_Original;
        UnityEngine::Color lightColor0Boost_Original;
        UnityEngine::Color lightColor1Boost_Original;

        SimpleColorSO* lightColor0;
        SimpleColorSO* lightColor1;
        SimpleColorSO* lightColor0Boost;
        SimpleColorSO* lightColor1Boost;

        MultipliedColorSO* mLightColor0;
        MultipliedColorSO* mHighlightColor0;
        MultipliedColorSO* mLightColor1;
        MultipliedColorSO* mHighlightColor1;

        MultipliedColorSO* mLightColor0Boost;
        MultipliedColorSO* mHighlightColor0Boost;
        MultipliedColorSO* mLightColor1Boost;
        MultipliedColorSO* mHighlightColor1Boost;

        bool supportBoostColor;

        float lastValue;

        LSEColorManager(MonoBehaviour* mono, BeatmapEventType type) {
            this->lse = mono;
            this->type = type;
            InitializeSOs(mono, "_lightColor0", lightColor0, lightColor0_Original, mLightColor0);
            InitializeSOs(mono, "_highlightColor0", lightColor0, lightColor0_Original, mHighlightColor0);
            InitializeSOs(mono, "_lightColor1", lightColor1, lightColor1_Original, mLightColor1);
            InitializeSOs(mono, "_highlightColor1", lightColor1, lightColor1_Original, mHighlightColor1);

            if (mono && mono->klass == classof(LightSwitchEventEffect*)) {
                auto* lse = reinterpret_cast<LightSwitchEventEffect*>(mono);
                InitializeSOs(mono, "_lightColor0Boost", lightColor0, lightColor0_Original, mLightColor0);
                InitializeSOs(mono, "_highlightColor0Boost", lightColor0, lightColor0_Original, mHighlightColor0);
                InitializeSOs(mono, "_lightColor1Boost", lightColor1, lightColor1_Original, mLightColor1);
                InitializeSOs(mono, "_highlightColor1Boost", lightColor1, lightColor1_Original, mHighlightColor1);
                supportBoostColor = true;

                Lights = lse->lightManager->lights->values[lse->lightsID];
                map<int, List_1<ILightWithId*>*> lightsPreGroup;
                if (!Lights) {
                    getLogger().info("LIGHTS IS NULL!!!");
                } else {
                    getLogger().info("LIGHTS ISNT NULL!!!");
                    getLogger().info(to_utf8(csstrtostr(Lights->GetType()->get_FullName())));
                }
                if (!Lights->items) {
                    getLogger().info("LIGHTS ITEMS IS NULL!!!");
                } else {
                    getLogger().info("LIGHTS ITEMS ISNT NULL!!!");
                }
                for (int i = 0; i < Lights->get_Count(); i++) {
                    //ILightWithId* light = Lights->get_Item(i);
                    //if (light/* && il2cpp_functions::class_has_parent(reinterpret_cast<Il2CppObject*>(light)->klass, classof(MonoBehaviour*))*/) {
                        getLogger().info("LIGHT FOUND!!");
                        //auto* monoBehaviour = reinterpret_cast<MonoBehaviour*>(light);
                        //int z = UnityEngine::Mathf::RoundToInt(monoBehaviour->get_transform()->get_position().z);
                        /*auto it = lightsPreGroup.find(z);
                        if (it != lightsPreGroup.end()) {
                            it->second->Add(light);
                        } else {
                            lightsPreGroup[z] = List_1<ILightWithId*>::New_ctor();
                            lightsPreGroup[z]->Add(light);
                        }*/
                    //}
                }
                
                /*for (auto elem : lightsPreGroup) {
                    if (!elem.second) {
                        continue;
                    }

                    Array<ILightWithId*>* lightList = elem.second->ToArray();

                    LightsPropagationGrouped.push_back(lightList);
                }*/
            }
        }

    public:
        List_1<ILightWithId*>* Lights;
        vector<Array<ILightWithId*>*> LightsPropagationGrouped;

        static unordered_set<LSEColorManager*> GetLSEColorManager(BeatmapEventType type) {
            unordered_set<LSEColorManager*> res;

            for (auto elem : lseColorManagers) {
                if (elem->type == type) {
                    res.insert(elem);
                }
            }

            return res;
        }

        static LSEColorManager* GetLSEColorManager(MonoBehaviour* lse) {
            for (auto elem : lseColorManagers) {
                if (elem->lse == lse) {
                    return elem;
                }
            }

            return nullptr;
        }

        static LSEColorManager* CreateLSEColorManager(MonoBehaviour* lse, BeatmapEventType type) {
            LSEColorManager* lsecm = new LSEColorManager(lse, type);
            lseColorManagers.insert(lsecm);
            return lsecm;
        }

        void Reset() {
            lightColor0->SetColor(lightColor0_Original);
            lightColor1->SetColor(lightColor1_Original);
            if (supportBoostColor) {
                lightColor0Boost->SetColor(lightColor0Boost_Original);
                lightColor1Boost->SetColor(lightColor1Boost_Original);
            }
        }

        void SetLightingColors(UnityEngine::Color* color0, UnityEngine::Color* color1, UnityEngine::Color* color0Boost = nullptr, UnityEngine::Color* color1Boost = nullptr) {
            if (!color0) {
                lightColor0->SetColor(*color0);
            }

            if (!color1) {
                lightColor1->SetColor(*color1);
            }

            if (supportBoostColor) {
                if (!color0Boost) {
                    lightColor0Boost->SetColor(*color0Boost);
                }

                if (!color1Boost) {
                    lightColor1Boost->SetColor(*color1Boost);
                }
            }
        }

        void SetLastValue(int value) {
            lastValue = value;
        }

        void SetActiveColors() {
            if (lastValue == 0) {
                return;
            }

            bool warm;
            switch ((int)lastValue)
            {
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
            switch ((int)lastValue)
            {
                case 1:
                case 5:
                default:
                    c = warm ? mLightColor0->get_color() : mLightColor1->get_color();
                    break;

                case 2:
                case 6:
                case 3:
                case 7:
                    c = warm ? mHighlightColor0->get_color() : mHighlightColor1->get_color();
                    break;
            }

            if (lse->get_enabled()) {
                CRASH_UNLESS(il2cpp_utils::SetFieldValue(lse, "_highlightColor", c));

                if (lastValue == 3 || lastValue == 7) {
                    c.a = 0.0f;

                    CRASH_UNLESS(il2cpp_utils::SetFieldValue(lse, "_afterHighlightColor", c));
                } else {
                    CRASH_UNLESS(il2cpp_utils::SetFieldValue(lse, "_afterHighlightColor", c));
                }
            } else {
                if (lastValue == 1 || lastValue == 5 || lastValue == 2 || lastValue == 6) {
                    if (lse->klass == classof(LightSwitchEventEffect*)) {
                        auto* l4 = reinterpret_cast<LightSwitchEventEffect*>(lse);
                        l4->SetColor(c);
                    } else if (lse->klass == classof(ParticleSystemEventEffect*)) {
                        auto* p4 = reinterpret_cast<ParticleSystemEventEffect*>(lse);
                        p4->particleColor = c;
                        p4->RefreshParticles();
                    }
                }
            }
            
            c.a = 0.0f;

            CRASH_UNLESS(il2cpp_utils::SetFieldValue(lse, "_offColor", c));
        }

    private:
        void InitializeSOs(MonoBehaviour* lse, string id, SimpleColorSO* sColorSO, UnityEngine::Color originalColor, MultipliedColorSO* mColorSO) {
            MultipliedColorSO* lightMultSO = reinterpret_cast<MultipliedColorSO*>(RET_V_UNLESS(getLogger(), il2cpp_utils::GetFieldValue<ColorSO*>(lse, id)));

            UnityEngine::Color multiplierColor = lightMultSO->multiplierColor;
            SimpleColorSO* lightSO = lightMultSO->baseColor;
            originalColor = lightSO->color;

            if (!mColorSO) {
                mColorSO = UnityEngine::ScriptableObject::CreateInstance<MultipliedColorSO*>();
                mColorSO->multiplierColor = multiplierColor;

                if (!sColorSO) {
                    sColorSO = UnityEngine::ScriptableObject::CreateInstance<SimpleColorSO*>();
                    sColorSO->SetColor(originalColor);
                }

                mColorSO->baseColor = sColorSO;
            }

            //CRASH_UNLESS(il2cpp_utils::SetFieldValue(lse, id, mColorSO));
        }
    };

public:
    static void LSEStart(MonoBehaviour* lse, BeatmapEventType type) {
        getLogger().info("LightColorizer::LSEStart() called.");

        LSEColorManager::CreateLSEColorManager(lse, type);
    }
};

unordered_set<LightColorizer::LSEColorManager*> LightColorizer::lseColorManagers = {};