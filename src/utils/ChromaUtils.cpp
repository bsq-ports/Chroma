#include "utils/ChromaUtils.hpp"
#include "modloader/shared/modloader.hpp"

using namespace Chroma;
using namespace ChromaUtils;

bool ChromaUtilities::IsModInstalled(std::string mod)
{
    return Modloader::getMods().contains(modName);
}

std::optional<UnityEngine::Color> ChromaUtilities::GetColorFromData(rapidjson::Value* data, const std::string& member)
{
    rapidjson::Value::MemberIterator color = data->FindMember(member);

    if (color == data->MemberEnd())
        return std::nullopt;


    // TODO: Will this work? Probably not
    return UnityEngine::Color(color->value[0].GetFloat(), color->value[1].GetFloat(), color->value[2].GetFloat(), color->value.MemberCount() > 3 ? color->value[3].GetFloat() : 1);
}

// internal
void ChromaUtilities::SetSongCoreCapability(std::string capability, bool enabled)
{
    // TODO: Will this be implemented ever?
    //    if (enabled)
    //    {
    //        SongCore.Collections.RegisterCapability(capability);
    //    }
    //    else
    //    {
    //        SongCore.Collections.DeregisterizeCapability(capability);
    //    }
}