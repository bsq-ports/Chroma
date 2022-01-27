#pragma once

#include "main.hpp"
#include "ChromaHooks.hpp"
#include "UnityEngine/Color.hpp"
#include <optional>
#include "GlobalNamespace/ILightWithId.hpp"
#include "UnityEngine/Quaternion.hpp"
#include <vector>
#include <string>

#define GET_FIND_METHOD(mPtr) il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::get()

template<auto val>
using FPtrWrapper = il2cpp_utils::il2cpp_type_check::FPtrWrapper<val>;

inline void PrintJSONValue(const rapidjson::Value &json) {
    #if DEBUGB == 1
        return;
    #endif
    using namespace rapidjson;

    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    json.Accept(writer);
    auto str = sb.GetString();
    getLogger().info("%s", str);
}

inline void PrintJSONValue(const rapidjson::Value* json) {
    #if DEBUGB == 1
        return;
    #endif
    using namespace rapidjson;

    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    json->Accept(writer);
    auto str = sb.GetString();
    getLogger().info("json %s", str);
}

namespace Chroma {
    namespace ChromaLogger {
        inline static const  char* ColorLightSwitch = "ColorLightSwitch";
        inline static const  char* EnvironmentRemoval = "EnvironmentRemoval";
        inline static const  char* LightColorizer = "LightColorizer";
        inline static const  char* TrackLaneRings = "TrackLaneRings";
        inline static const  char* LegacyLightColor = "LegacyLightColor";
        inline static const  char* ObjectDataDeserialize = "ObjectDataDeserialize";
        inline static const  char* TrackController = "GameObjectTrackController";
    }

    inline static const  std::u16string_view REQUIREMENTNAME = u"Chroma";


    inline static const  std::string_view ANIMATION = "_animation";
    inline static const  std::string_view TRACK = "_track";
    inline static const  std::string_view DIRECTION = "_direction";
    inline static const  std::string_view COLOR = "_color";
    inline static const  std::string_view COUNTERSPIN = "_counterSpin";
    inline static const  std::string_view DURATION = "_duration";
    inline static const  std::string_view EASING = "_easing";
    inline static const  std::string_view LERP_TYPE = "_lerpType";
    inline static const  std::string_view STARTCOLOR = "_startColor";
    inline static const  std::string_view ENDCOLOR = "_endColor";
    inline static const  std::string_view PRECISESPEED = "_preciseSpeed";
    inline static const  std::string_view LOCKPOSITION = "_lockPosition";
    inline static const  std::string_view PROPAGATIONID = "_propID";
    inline static const  std::string_view LIGHTID = "_lightID";
    inline static const  std::string_view DISABLESPAWNEFFECT = "_disableSpawnEffect";
    inline static const  std::string_view NAMEFILTER = "_nameFilter";
    inline static const  std::string_view RESET = "_reset";
    inline static const  std::string_view STEP = "_step";
    inline static const  std::string_view SPEED = "_speed";
    inline static const  std::string_view SPEEDMULT = "_speedMult";
    inline static const  std::string_view PROPMULT = "_propMult";
    inline static const  std::string_view STEPMULT = "_stepMult";
    inline static const  std::string_view PROP = "_prop";
    inline static const  std::string_view ROTATION = "_rotation";
    inline static const  std::string_view LIGHTGRADIENT = "_lightGradient";

    inline static const  std::u16string ENVIRONMENTREMOVAL = u"_environmentRemoval";
    inline static const  std::string_view ENVIRONMENT = "_environment";
    inline static const  std::string_view IDVAR = "_id";
    inline static const  std::string_view LOOKUPMETHOD = "_lookupMethod";
    inline static const  std::string_view DUPLICATIONAMOUNT = "_duplicate";
    inline static const  std::string_view ACTIVE = "_active";
    inline static const  std::string_view SCALE = "_scale";
    inline static const  std::string_view POSITION = "_position";
    inline static const  std::string_view LOCALPOSITION = "_localPosition";
    inline static const  std::string_view OBJECTROTATION = "_rotation";
    inline static const  std::string_view LOCALROTATION = "_localRotation";

    inline static const  std::string_view ASSIGNFOGTRACK = "AssignFogTrack";
}