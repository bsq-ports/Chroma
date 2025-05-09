#pragma once

#include "main.hpp"
#include "ChromaHooks.hpp"
#include "tracks/shared/AssociatedData.h"

#include <string_view>

template <auto val> using FPtrWrapper = il2cpp_utils::il2cpp_type_check::FPtrWrapper<val>;


namespace Chroma {

void PrintJSONValue(rapidjson::Value const& json);
void PrintJSONValue(rapidjson::Value const* json);

inline static const std::string REQUIREMENTNAME = "Chroma";

namespace NewConstants {
using namespace TracksAD::Constants;

inline static constexpr const std::string_view V2_COLOR = "_color";
inline static constexpr const std::string_view V2_COUNTER_SPIN = "_counterSpin";
inline static constexpr const std::string_view V2_DIRECTION = "_direction";
inline static constexpr const std::string_view V2_DISABLE_SPAWN_EFFECT = "_disableSpawnEffect";
inline static constexpr const std::string_view V2_END_COLOR = "_endColor";
inline static constexpr const std::u16string_view V2_ENVIRONMENT_REMOVAL = u"_environmentRemoval";
inline static constexpr const std::string_view V2_LERP_TYPE = "_lerpType";
inline static constexpr const std::string_view V2_LIGHT_GRADIENT = "_lightGradient";
inline static constexpr const std::string_view V2_LIGHT_ID = "_lightID";
inline static constexpr const std::string_view V2_LOCK_POSITION = "_lockPosition";
inline static constexpr const std::string_view V2_NAME_FILTER = "_nameFilter";
inline static constexpr const std::string_view V2_PRECISE_SPEED = "_preciseSpeed";
inline static constexpr const std::string_view V2_PROP = "_prop";
inline static constexpr const std::string_view V2_PROPAGATION_ID = "_propID";
inline static constexpr const std::string_view V2_PROP_MULT = "_propMult";
inline static constexpr const std::string_view V2_RESET = "_reset";
inline static constexpr const std::string_view V2_SPEED = "_speed";
inline static constexpr const std::string_view V2_SPEED_MULT = "_speedMult";
inline static constexpr const std::string_view V2_START_COLOR = "_startColor";
inline static constexpr const std::string_view V2_STEP = "_step";
inline static constexpr const std::string_view V2_STEP_MULT = "_stepMult";
inline static constexpr const std::string_view V2_LOCAL_POSITION = "_localPosition";

inline static constexpr const std::string_view V2_ENVIRONMENT = "_environment";
inline static constexpr const std::string_view V2_GAMEOBJECT_ID = "_id";
inline static constexpr const std::string_view V2_LOOKUP_METHOD = "_lookupMethod";
inline static constexpr const std::string_view V2_DUPLICATION_AMOUNT = "_duplicate";
inline static constexpr const std::string_view V2_ACTIVE = "_active";

inline static constexpr const std::string_view V2_ATTENUATION = "_attenuation";
inline static constexpr const std::string_view V2_OFFSET = "_offset";
inline static constexpr const std::string_view V2_HEIGHT_FOG_STARTY = "_startY";
inline static constexpr const std::string_view V2_HEIGHT_FOG_HEIGHT = "_height";

inline static constexpr const std::string_view V2_GEOMETRY = "_geometry";
inline static constexpr const std::string_view V2_GEOMETRY_TYPE = "_type";
inline static constexpr const std::string_view V2_SHADER_PRESET = "_shader";
inline static constexpr const std::string_view V2_SHADER_KEYWORDS = "_shaderKeywords";
inline static constexpr const std::string_view V2_COLLISION = "_collision";
inline static constexpr const std::string_view V2_MATERIALS = "_materials";
inline static constexpr const std::string_view V2_MATERIAL = "_material";

inline static constexpr const std::string_view COLOR = "color";
inline static constexpr const std::string_view DIRECTION = "direction";
inline static constexpr const std::string_view NOTE_SPAWN_EFFECT = "spawnEffect";
inline static constexpr const std::string_view LERP_TYPE = "lerpType";
inline static constexpr const std::string_view LIGHT_ID = "lightID";
inline static constexpr const std::string_view LIGHT_TYPE = "type";
inline static constexpr const std::string_view LOCK_POSITION = "lockRotation";
inline static constexpr const std::string_view NAME_FILTER = "nameFilter";
inline static constexpr const std::string_view PROP = "prop";
inline static constexpr const std::string_view SPEED = "speed";
inline static constexpr const std::string_view STEP = "step";
inline static constexpr const std::string_view RING_ROTATION = "rotation";

inline static constexpr const std::string_view ENVIRONMENT = "environment";
inline static constexpr const std::string_view GAMEOBJECT_ID = "id";
inline static constexpr const std::string_view GEOMETRY = "geometry";
inline static constexpr const std::string_view MATERIAL = "material";
inline static constexpr const std::string_view MATERIALS = "materials";
inline static constexpr const std::string_view GEOMETRY_TYPE = "type";
inline static constexpr const std::string_view SHADER_PRESET = "shader";
inline static constexpr const std::string_view SHADER_KEYWORDS = "shaderKeywords";
inline static constexpr const std::string_view COLLISION = "collision";
inline static constexpr const std::string_view LOOKUP_METHOD = "lookupMethod";
inline static constexpr const std::string_view DUPLICATION_AMOUNT = "duplicate";
inline static constexpr const std::string_view ACTIVE = "active";
inline static constexpr const std::string_view LOCAL_POSITION = "localPosition";

inline static constexpr const std::string_view ATTENUATION = "attenuation";
inline static constexpr const std::string_view OFFSET = "offset";
inline static constexpr const std::string_view HEIGHT_FOG_STARTY = "startY";
inline static constexpr const std::string_view HEIGHT_FOG_HEIGHT = "height";

inline static constexpr const std::string_view ANIMATE_COMPONENT = "AnimateComponent";

inline static constexpr const std::string_view COMPONENTS = "components";
inline static constexpr const std::string_view LIGHT_WITH_ID = "ILightWithId";
inline static constexpr const std::string_view BLOOM_FOG_ENVIRONMENT = "BloomFogEnvironment";
inline static constexpr const std::string_view TUBE_BLOOM_PRE_PASS_LIGHT = "TubeBloomPrePassLight";
inline static constexpr const std::string_view COLOR_ALPHA_MULTIPLIER = "colorAlphaMultiplier";
inline static constexpr const std::string_view BLOOM_FOG_INTENSITY_MULTIPLIER = "bloomFogIntensityMultiplier";
} // namespace NewConstants

namespace OldConstants {
inline static constexpr const std::string_view COUNTERSPIN = "_counterSpin";
inline static constexpr const std::string_view STARTCOLOR = "_startColor";
inline static constexpr const std::string_view ENDCOLOR = "_endColor";
inline static constexpr const std::string_view PRECISESPEED = "_preciseSpeed";
inline static constexpr const std::string_view LOCKPOSITION = "_lockPosition";
inline static constexpr const std::string_view PROPAGATIONID = "_propID";
inline static constexpr const std::string_view LIGHTID = "_lightID";
inline static constexpr const std::string_view DISABLESPAWNEFFECT = "_disableSpawnEffect";
inline static constexpr const std::string_view NAMEFILTER = "_nameFilter";
inline static constexpr const std::string_view RESET = "_reset";
inline static constexpr const std::string_view SPEEDMULT = "_speedMult";
inline static constexpr const std::string_view PROPMULT = "_propMult";
inline static constexpr const std::string_view STEPMULT = "_stepMult";
inline static constexpr const std::string_view LIGHTGRADIENT = "_lightGradient";

inline static constexpr const std::u16string_view ENVIRONMENTREMOVAL = u"_environmentRemoval";
inline static constexpr const std::string_view IDVAR = "_id";
inline static constexpr const std::string_view LOOKUPMETHOD = "_lookupMethod";
inline static constexpr const std::string_view DUPLICATIONAMOUNT = "_duplicate";
inline static constexpr const std::string_view LOCALPOSITION = "_localPosition";
inline static constexpr const std::string_view OBJECTROTATION = "_rotation";
inline static constexpr const std::string_view LOCALROTATION = "_localRotation";

inline static constexpr const std::string_view ASSIGNFOGTRACK = "AssignFogTrack";
} // namespace OldConstants
} // namespace Chroma