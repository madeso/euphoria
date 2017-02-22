// Copyright (2015) Gustav

#include "core/proto.h"

#include <google/protobuf/text_format.h>

#include <streambuf>
#include "core/assert.h"
#include <fstream>  // NOLINT this is how we use fstrean
#include <sstream>  // NOLINT this is how we use sstream

#include <string>

#include "pbjson.hpp"  // NOLINT this is how we use pbjson

// #include "ride/stringutils.h"

bool LoadProtoText(google::protobuf::Message* t, const std::string& file_name) {
  assert(t);
  std::ifstream file(file_name.c_str());
  if (!file) return false;
  std::string data((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());
  if (false == google::protobuf::TextFormat::ParseFromString(data, t))
    return false;
  return true;
}

bool SaveProtoText(const google::protobuf::Message& t,
                   const std::string& file_name) {
  // if (false == VerifyFileForWriting(file_name)) return false;
  const std::string path = file_name;

  std::ofstream output(path.c_str());
  std::string data;
  if (false == google::protobuf::TextFormat::PrintToString(t, &data))
    return false;
  output << data;
  return true;
}

bool LoadProtoBinary(google::protobuf::Message* message,
                     const std::string& path) {
  std::fstream input(path.c_str(), std::ios::in | std::ios::binary);
  const bool parse_result = message->ParseFromIstream(&input);
  if (false == parse_result) {
    return false;
  }

  return true;
}

bool SaveProtoBinary(const google::protobuf::Message& message,
                     const std::string& config_path) {
  std::fstream config_stream(
      config_path.c_str(),
      std::ios::out | std::ios::trunc | std::ios::binary);
  return message.SerializeToOstream(&config_stream);
}

std::string LoadProtoJson(google::protobuf::Message* message,
                       const std::string& path) {
  std::string err;
  int load_result = pbjson::json2pb_file(path.c_str(), message, err);
  if (load_result < 0) {
    return err.c_str();
  }

  return "";
}

std::string SaveProtoJson(const google::protobuf::Message& t,
                       const std::string& path) {
  bool write_result = pbjson::pb2json_file(&t, path.c_str(), true);
  if (write_result == false) {
    return "Unable to write to file";
  }

  return "";
}
