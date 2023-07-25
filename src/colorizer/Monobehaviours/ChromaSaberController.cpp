#include "colorizer/Monobehaviours/ChromaSaberController.hpp"

DEFINE_TYPE(Chroma, ChromaSaberController);

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;

void Chroma::ChromaSaberController::Init(GlobalNamespace::Saber* saber) {
  _saberType = saber->get_saberType();
  _colorizer = &SaberColorizer::New(saber);
  _saberModelController = _colorizer->getSaberModelController();
}

void Chroma::ChromaSaberController::OnDestroy() {
  SaberColorizer::RemoveColorizer(_saberModelController);
}
