// Copyright (2015) Gustav

#include "core/proto.h"

#include "core/assert.h"
#include "core/vfs.h"
#include "core/str.h"

#include <rapidjson/error/en.h>
#include <rapidjson/stream.h>
#include <rapidjson/cursorstreamwrapper.h>

#include <fstream>

namespace euphoria::core
{

std::string
LoadProtoJson_Internal(
    vfs::FileSystem* fs, rapidjson::Document* doc, const std::string& file_name)
{
  std::string source;

  const bool load_result = fs->ReadFileToString(file_name, &source);
  if(!load_result)
  {
    return Str() << "Unable to load file " << file_name << " from "
                 << fs->GetRootsAsString();
  }

  // todo: look up insitu parsing
  // todo: look upo json/sjson parsing options

  using InputStream = rapidjson::CursorStreamWrapper<rapidjson::StringStream>;
  rapidjson::StringStream ss{source.c_str()};
  InputStream             stream{ss};
  constexpr unsigned int  ParseFlags =
      rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag;
  doc->ParseStream<ParseFlags, rapidjson::UTF8<>, InputStream>(stream);

  if(doc->HasParseError())
  {
    // todo: add file and parse error to error
    return Str{} << "JSON parse error(" << stream.GetLine() << ":"
                 << stream.GetColumn()
                 << "): " << rapidjson::GetParseError_En(doc->GetParseError());
  }

#if 0
  std::string err;
  return protojson::ToProto(doc, message);
#else
  return "";
#endif
}

std::string
SaveProtoJson(
    const google::protobuf::Message& message, const std::string& file_name)
{
#if 0
  bool write_result = pbjson::pb2json_file(&message, file_name, true);
  if(!write_result)
  {
    return "Unable to write to file";
  }
#endif

  return "";
}

}
