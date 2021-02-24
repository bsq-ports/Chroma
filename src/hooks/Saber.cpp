#include "Chroma.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/Saber.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "colorizer/SaberColorizer.hpp"
#include "UnityEngine/Random.hpp"
#include "UnityEngine/Space.hpp"
#include "UnityEngine/Transform.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

MAKE_HOOK_OFFSETLESS(Saber_Start,void,Saber* self) {
    SaberColorizer::BSMStart(self, self->get_saberType().value);
    Saber_Start(self);
}

void Chroma::Hooks::Saber() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), Saber_Start, il2cpp_utils::FindMethodUnsafe("", "Saber", "Start", 0));
}