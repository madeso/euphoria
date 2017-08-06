#ifndef SPACETYPER_PROTO_H_
#define SPACETYPER_PROTO_H_

#include <string>

#include <google/protobuf/message.h>

class FileSystem;

bool LoadProtoText(google::protobuf::Message* t, const std::string& path);
bool SaveProtoText(const google::protobuf::Message& t, const std::string& path);

bool LoadProtoBinary(google::protobuf::Message* message,
                     const std::string& file_name);
bool SaveProtoBinary(const google::protobuf::Message& message,
                     const std::string& file_name);

std::string LoadProtoJson(FileSystem* fs, google::protobuf::Message* t, const std::string& path);
std::string SaveProtoJson(const google::protobuf::Message& t,
                       const std::string& path);

#endif // SPACETYPER_PROTO_H_
