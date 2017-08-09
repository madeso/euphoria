#ifndef EUPHORIA_PROTOJSON_H
#define EUPHORIA_PROTOJSON_H

#include <string>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include "rapidjson/document.h"

namespace protojson
{
  std::string ToProto(const rapidjson::Value& json, google::protobuf::Message* msg);
}


#endif //EUPHORIA_PROTOJSON_H
