#include "utils/ChromaUtils.hpp"
#include "modloader/shared/modloader.hpp"

using namespace Chroma;
using namespace ChromaUtils;

std::optional<Sombrero::FastColor> ChromaUtilities::GetColorFromData(std::optional<std::reference_wrapper<rapidjson::Value>> data, const std::string& member)
{
    if (!data) return std::nullopt;

    rapidjson::Value& unwrapped = *data;

    rapidjson::Value::MemberIterator color = unwrapped.FindMember(member);

    if (color == unwrapped.MemberEnd())
        return std::nullopt;


    // TODO: Will this work? Probably not
    return Sombrero::FastColor(color->value[0].GetFloat(), color->value[1].GetFloat(), color->value[2].GetFloat(), color->value.MemberCount() > 3 ? color->value[3].GetFloat() : 1);
}

std::optional<Sombrero::FastColor> ChromaUtilities::GetColorFromData(rapidjson::Value& data, const std::string& member)
{
    if (data.GetType() == rapidjson::kNullType || data.Empty()) return std::nullopt;

    rapidjson::Value::MemberIterator color = data.FindMember(member);

    if (color == data.MemberEnd())
        return std::nullopt;


    // TODO: Will this work? Probably not
    return Sombrero::FastColor(color->value[0].GetFloat(), color->value[1].GetFloat(), color->value[2].GetFloat(), color->value.MemberCount() > 3 ? color->value[3].GetFloat() : 1);
}