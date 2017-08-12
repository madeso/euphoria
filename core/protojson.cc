#include <iostream>
#include <google/protobuf/descriptor.pb.h>
#include "protojson.h"
#include "core/assert.h"
#include "core/str.h"
#include "stringmerger.h"

// todo: add proto-> json
// todo: fix naming

namespace
{
  const char* const
  TypeToString(rapidjson::Type type)
  {
    switch(type)
    {
      case rapidjson::kNullType:
        return "null";
      case rapidjson::kFalseType:
        return "false";
      case rapidjson::kTrueType:
        return "true";
      case rapidjson::kObjectType:
        return "object";
      case rapidjson::kArrayType:
        return "array";
      case rapidjson::kStringType:
        return "string";
      case rapidjson::kNumberType:
        return "number";
      default:
        return "<unknown>";
    }
  }

  std::string
  ToString(const rapidjson::Value& val)
  {
    // todo: add more printing
    return TypeToString(val.GetType());
  }

  class JsonPath
  {
   public:
    JsonPath(const JsonPath* p, const rapidjson::Value& s)
        : parent(p)
        , self(ToString(s))
    {
    }

    static void
    Print(const JsonPath* path, std::ostringstream& ss)
    {
      if(path != nullptr)
      {
        Print(path->parent, ss);
        ss << "/" << path->self;
      }
      else
      {
        // todo: print root sign?
        ss << "<root>";
      }
    }

   private:
    const JsonPath* parent;
    std::string     self;
  };

  std::ostringstream&
  operator<<(std::ostringstream& o, const JsonPath& path)
  {
    JsonPath::Print(&path, o);
    return o;
  }


  std::string
  JsonToProtoField(const rapidjson::Value& json, google::protobuf::Message* msg,
                   const google::protobuf::FieldDescriptor* field,
                   const JsonPath*                          path)
  {
    const JsonPath self_path{path, json};
    const auto*    ref      = msg->GetReflection();
    const bool     repeated = field->is_repeated();
    switch(field->cpp_type())
    {
      case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
      {
        if(json.GetType() != rapidjson::kNumberType)
        {
          return Str() << "Not a number(int32), found a "
                       << TypeToString(json.GetType()) << " at " << self_path;
        }
        if(repeated)
        {
          ref->AddInt32(msg, field, (int32_t)json.GetInt());
        }
        else
        {
          ref->SetInt32(msg, field, (int32_t)json.GetInt());
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
      {
        if(json.GetType() != rapidjson::kNumberType)
        {
          return Str() << "Not a number(uint32), found a "
                       << TypeToString(json.GetType()) << " at " << self_path;
        }
        if(repeated)
        {
          ref->AddUInt32(msg, field, json.GetUint());
        }
        else
        {
          ref->SetUInt32(msg, field, json.GetUint());
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
      {
        if(json.GetType() != rapidjson::kNumberType)
        {
          return Str() << "Not a number(int64), found a "
                       << TypeToString(json.GetType()) << " at " << self_path;
        }
        if(repeated)
        {
          ref->AddInt64(msg, field, json.GetInt64());
        }
        else
        {
          ref->SetInt64(msg, field, json.GetInt64());
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
      {
        if(json.GetType() != rapidjson::kNumberType)
        {
          return Str() << "Not a number(uint64), found a "
                       << TypeToString(json.GetType()) << " at " << self_path;
        }
        if(repeated)
        {
          ref->AddUInt64(msg, field, json.GetUint64());
        }
        else
        {
          ref->SetUInt64(msg, field, json.GetUint64());
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
      {
        if(json.GetType() != rapidjson::kNumberType)
        {
          return Str() << "Not a number(double), found a "
                       << TypeToString(json.GetType()) << " at " << self_path;
        }
        if(repeated)
        {
          ref->AddDouble(msg, field, json.GetDouble());
        }
        else
        {
          ref->SetDouble(msg, field, json.GetDouble());
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
      {
        if(json.GetType() != rapidjson::kNumberType)
        {
          return Str() << "Not a number(float), found a "
                       << TypeToString(json.GetType()) << " at " << self_path;
        }
        if(repeated)
        {
          ref->AddFloat(msg, field, json.GetDouble());
        }
        else
        {
          ref->SetFloat(msg, field, json.GetDouble());
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
      {
        if(json.GetType() != rapidjson::kTrueType &&
           json.GetType() != rapidjson::kFalseType)
        {
          return Str() << "Not true or false, found a "
                       << TypeToString(json.GetType()) << " at " << self_path;
        }
        bool v = json.GetBool();
        if(repeated)
        {
          ref->AddBool(msg, field, v);
        }
        else
        {
          ref->SetBool(msg, field, v);
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
      {
        if(json.GetType() != rapidjson::kStringType)
        {
          return Str() << "Not a string, found a "
                       << TypeToString(json.GetType()) << " at " << self_path;
        }
        const char* value    = json.GetString();
        uint32_t    str_size = json.GetStringLength();
        std::string str_value(value, str_size);
        if(field->type() == google::protobuf::FieldDescriptor::TYPE_BYTES)
        {
          if(repeated)
          {
            Assert(false);
            // ref->AddString(msg, field, b64_decode(str_value));
          }
          else
          {
            Assert(false);
            // ref->SetString(msg, field, b64_decode(str_value));
          }
        }
        else
        {
          if(repeated)
          {
            ref->AddString(msg, field, str_value);
          }
          else
          {
            ref->SetString(msg, field, str_value);
          }
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
      {
        google::protobuf::Message* mf = (repeated)
                                            ? ref->AddMessage(msg, field)
                                            : ref->MutableMessage(msg, field);
        return protojson::ToProto(json, mf);
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
      {
        const auto*                                  ed = field->enum_type();
        const google::protobuf::EnumValueDescriptor* ev = nullptr;
        if(json.GetType() == rapidjson::kNumberType)
        {
          const auto value = json.GetInt();
          ev               = ed->FindValueByNumber(value);
          if(!ev)
          {
            return Str() << "Enum value " << value << "not found, "
                         << " at " << self_path;
          }
        }
        else if(json.GetType() == rapidjson::kStringType)
        {
          const std::string value = json.GetString();
          ev                      = ed->FindValueByName(value);
          if(!ev)
          {
            // todo: list all values, guess based on shortest edit distance?
            return Str() << "Enum value " << value << "not found, "
                         << " at " << self_path;
          }
        }
        else
        {
          return Str() << "Neither integer or string(enum), found a "
                       << TypeToString(json.GetType()) << " at " << self_path;
        }
        if(repeated)
        {
          ref->AddEnum(msg, field, ev);
        }
        else
        {
          ref->SetEnum(msg, field, ev);
        }
        break;
      }
      default:
        return Str() << "Unhandled json type, found a "
                     << TypeToString(json.GetType()) << " at " << self_path;
    }
    return "";
  }

  std::vector<std::string>
  FieldNamesToList(const google::protobuf::Descriptor* descriptor)
  {
    std::vector<std::string> ret;
    for(int field_id = 0; field_id < descriptor->field_count(); field_id += 1)
    {
      const auto* field = descriptor->field(field_id);
      ret.push_back(field->name());
    }
    return ret;
  }
}

namespace protojson
{
  std::string
  ToProto(const rapidjson::Value& json, google::protobuf::Message* msg,
          JsonPath* path)
  {
    JsonPath self_path{path, json};
    if(json.GetType() != rapidjson::kObjectType)
    {
      return Str() << "JSON root is not a object at " << self_path;
      ;
    }
    const auto* descriptor = msg->GetDescriptor();
    const auto* reflection = msg->GetReflection();
    if(descriptor == nullptr || reflection == nullptr)
    {
      return Str() << "Protobuf object has no reflection at " << self_path;
    }

    for(rapidjson::Value::ConstMemberIterator itr = json.MemberBegin();
        itr != json.MemberEnd(); ++itr)
    {
      const char* name  = itr->name.GetString();
      const auto* field = descriptor->FindFieldByName(name);
      if(field == nullptr)
      {
        field = reflection->FindKnownExtensionByName(name);
      }
      if(field == nullptr)
      {
        // todo: list all fields
        return Str() << "Failed to find json field " << name << " in proto "
                     << descriptor->name() << " at " << self_path
                     << ". Value could be "
                     << StringMerger::EnglishOr().Generate(
                            FieldNamesToList(descriptor));
      }
      if(itr->value.GetType() == rapidjson::kNullType)
      {
        reflection->ClearField(msg, field);
        continue;
      }
      if(field->is_repeated())
      {
        if(itr->value.GetType() != rapidjson::kArrayType)
        {
          return Str()
                 << "Protobuf is repeated but json didn't specify an array at "
                 << self_path;
        }
        for(rapidjson::Value::ConstValueIterator ait = itr->value.Begin();
            ait != itr->value.End(); ++ait)
        {
          const rapidjson::Value* val = ait;
          const auto ret = JsonToProtoField(*val, msg, field, &self_path);
          if(!ret.empty())
          {
            return ret;
          }
        }
      }
      else
      {
        const auto ret = JsonToProtoField(itr->value, msg, field, &self_path);
        if(!ret.empty())
        {
          return ret;
        }
      }
    }
    return "";
  }

  std::string
  ToProto(const rapidjson::Value& json, google::protobuf::Message* msg)
  {
    return ToProto(json, msg, nullptr);
  }
}
