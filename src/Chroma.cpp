#include "Chroma.hpp"
#include "ChromaLogger.hpp"

void Chroma::PrintJSONValue(rapidjson::Value const* json) {
  if (json == nullptr) {
    return;
  }

  using namespace rapidjson;

  StringBuffer sb;
  PrettyWriter<StringBuffer> writer(sb);
  json->Accept(writer);
  auto const* str = sb.GetString();
  ChromaLogger::Logger.info("{}", str);
}

void Chroma::PrintJSONValue(rapidjson::Value const& json) {
  PrintJSONValue(&json);
}