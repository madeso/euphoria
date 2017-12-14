// Copyright (2015) Gustav

#include "core/proto.h"

#include "core/assert.h"
#include "core/filesystem.h"
#include "core/str.h"
#include "core/protojson.h"

#include <pbjson.hpp>
#include <rapidjson/error/en.h>

#include <google/protobuf/text_format.h>

#include <fstream>

bool
LoadProtoText(google::protobuf::Message* message, const std::string& file_name)
{
  ASSERT(message);
  std::ifstream file(file_name.c_str());
  if(!file)
  {
    return false;
  }
  std::string data(
      (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  const bool parse_result =
      google::protobuf::TextFormat::ParseFromString(data, message);
  return parse_result;
}

bool
SaveProtoText(
    const google::protobuf::Message& message, const std::string& file_name)
{
  // if (false == VerifyFileForWriting(file_name)) return false;

  std::ofstream output(file_name.c_str());
  std::string   data;
  const bool    written_to_string =
      google::protobuf::TextFormat::PrintToString(message, &data);
  if(!written_to_string)
  {
    return false;
  }
  output << data;
  return true;
}

bool
LoadProtoBinary(
    google::protobuf::Message* message, const std::string& file_name)
{
  std::fstream input(file_name.c_str(), std::ios::in | std::ios::binary);
  const bool   parse_result = message->ParseFromIstream(&input);
  return parse_result;
}

bool
SaveProtoBinary(
    const google::protobuf::Message& message, const std::string& file_name)
{
  std::fstream config_stream(
      file_name.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
  return message.SerializeToOstream(&config_stream);
}

std::string
LoadProtoJson(
    FileSystem*                fs,
    google::protobuf::Message* message,
    const std::string&         file_name)
{
  std::string source;

  const bool load_result = fs->ReadFileToString(file_name, &source);
  if(!load_result)
  {
    // todo: add file to error
    return "Unable to load file";
  }

  rapidjson::Document doc;
  // todo: look up insitu parsing
  // todo: look upo json/sjson parsing options
  doc.Parse(source.c_str());

  if(doc.HasParseError())
  {
    // todo: add file and parse error to error
    return Str{} << "JSON parse error: "
                 << rapidjson::GetParseError_En(doc.GetParseError());
  }

  std::string err;

  return protojson::ToProto(doc, message);
}

std::string
SaveProtoJson(
    const google::protobuf::Message& message, const std::string& file_name)
{
  bool write_result = pbjson::pb2json_file(&message, file_name, true);
  if(!write_result)
  {
    return "Unable to write to file";
  }

  return "";
}
