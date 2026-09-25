#!/usr/bin/env python3
"""Exercise the actual per-game cleanup; retained material/slider controls must fail."""
from pathlib import Path
import json, subprocess, tempfile
root=Path(__file__).resolve().parents[1]
s=(root/'src/ChromaController.cpp').read_text()
start=s.index('void ClearChromaGameplayReferences() {')
end=s.index('\nvoid ChromaController::OnActiveSceneChanged',start)
body=s[start:end]
prefix=r'''#include <vector>
#include <cassert>
namespace Paper {enum class LogLevel {INF};}
struct Log {template<Paper::LogLevel L,class... T> void fmtLog(const char*,T...) {}};
namespace CJDLogger {inline Log Logger;}
struct ChromaGradientController {inline static int count=0;static void clearInstance(){count=0;}};
struct TrackLaneRingsManagerHolder {inline static std::vector<int> RingManagers;};
namespace Chroma::Component {inline int count=0;void ClearCoroutines(){count=0;}}
#define RESET_TYPE(T) struct T {inline static int count=0;static void Reset(){count=0;}};
RESET_TYPE(LightColorizer) RESET_TYPE(ObstacleColorizer) RESET_TYPE(BombColorizer)
RESET_TYPE(NoteColorizer) RESET_TYPE(SliderColorizer) RESET_TYPE(SaberColorizer)
RESET_TYPE(ParticleColorizer) RESET_TYPE(LightIdRegisterer) RESET_TYPE(MaterialsManager)
'''
suffix=r'''
int main(){
 for(int cycle=0;cycle<100;++cycle){
  ChromaGradientController::count=4;TrackLaneRingsManagerHolder::RingManagers={1,2,3};
  Chroma::Component::count=5;
  LightColorizer::count=ObstacleColorizer::count=BombColorizer::count=NoteColorizer::count=12;
  SliderColorizer::count=SaberColorizer::count=ParticleColorizer::count=LightIdRegisterer::count=MaterialsManager::count=13;
  ClearChromaGameplayReferences();
  assert(ChromaGradientController::count==0&&TrackLaneRingsManagerHolder::RingManagers.empty()&&Chroma::Component::count==0);
  assert(LightColorizer::count==0&&ObstacleColorizer::count==0&&BombColorizer::count==0&&NoteColorizer::count==0);
  assert(SliderColorizer::count==0&&SaberColorizer::count==0&&ParticleColorizer::count==0&&LightIdRegisterer::count==0&&MaterialsManager::count==0);
  ClearChromaGameplayReferences();
 }
}
'''
with tempfile.TemporaryDirectory(prefix='chroma-scene-cleanup-') as tmp:
 p=Path(tmp)
 def run(name,method):
  (p/(name+'.cpp')).write_text(prefix+method+suffix)
  subprocess.run(['c++','-std=c++20',str(p/(name+'.cpp')),'-o',str(p/name)],check=True)
  return subprocess.run([str(p/name)],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL).returncode
 assert run('production',body)==0
 assert run('negative-materials',body.replace('  MaterialsManager::Reset();',''))!=0
 assert run('negative-sliders',body.replace('  SliderColorizer::Reset();',''))!=0
print(json.dumps({'production_cleanup_body':True,'cleanup_cycles':100,'idempotence':True,'retained_material_and_slider_negative_controls_failed_as_expected':True,'passed':True}))
