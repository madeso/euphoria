#include "core/enum.h"

#include <iostream>
#include <set>
#include <utility>

#include "core/assert.h"
#include "core/proto.h"
#include "core/str.h"
#include "core/stringmerger.h"
#include "core/stringutils.h"
#include "enum.pb.h"

EnumType::EnumType(std::string name)
    : name_(std::move(name))
    , isAdding_(true)
    , nextIndex_(1)
{
}

EnumType::~EnumType()
{
  ASSERT(!isAdding_);
}

std::string
EnumType::ToString(size_t v) const
{
  ASSERT(v < nextIndex_);
  auto f = valueToName_.find(v);
  if(f != valueToName_.end())
  {
    return f->second;
  }

  // ASSERT(false && "Invalid index");
  const auto values =
      StringMerger::EnglishOr().Generate(MapToStringVector(nameToValue_));
  const std::string invalid = Str() << "<invalid value " << v << " of "
                                    << values << ">";
  return invalid;
}

const EnumValue
EnumType::ToEnum(const std::string& name)
{
  NameToValue::const_iterator r = nameToValue_.find(name);
  if(r != nameToValue_.end())
  {
    return EnumValue(this, r->second);
  }

  if(!isAdding_)
  {
    std::cerr << "Enum value doesnt exist, " << name << "\n";
    return EnumValue(this, 0);
  }
  const size_t id = nextIndex_;
  AddEnum(name);
  return EnumValue(this, id);
}

void
EnumType::AddEnums(const std::vector<std::string>& names)
{
  ASSERT(isAdding_);
  std::set<std::string> valid_names;
  for(const auto& name : names)
  {
    AddEnum(name);
    valid_names.insert(name);
  }

  // validate all names against true names
  for(const auto& name : nameToValue_)
  {
    const bool missing = valid_names.find(name.first) == valid_names.end();
    if(missing)
    {
      std::cerr << "Enum " << name_ << " was registered with name "
                << name.first << " but that is invalid.\n";
    }
  }

  isAdding_ = false;
}

void
EnumType::AddEnum(const std::string& name)
{
  ASSERT(isAdding_);
  NameToValue::const_iterator r = nameToValue_.find(name);
  if(r != nameToValue_.end())
  {
    return;
  }
  const size_t id = nextIndex_;
  ++nextIndex_;
  valueToName_.insert(ValueToName::value_type(id, name));
  nameToValue_.insert(NameToValue::value_type(name, id));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enum Value

EnumValue::EnumValue(EnumType* t, size_t v)
    : type_(t)
    , value_(v)
{
}

const std::string
EnumValue::ToString() const
{
  return type_->ToString(value_);
}

const size_t
EnumValue::ToValue() const
{
  return value_;
}

bool
EnumValue::operator==(const EnumValue& other) const
{
  ASSERT(type_ == other.type_);
  return value_ == other.value_;
}

bool
EnumValue::operator!=(const EnumValue& other) const
{
  return !(*this == other);
}

bool
EnumValue::operator<(const EnumValue& other) const
{
  ASSERT(type_ == other.type_);
  return value_ < other.value_;
}

std::ostream&
operator<<(std::ostream& s, const EnumValue& v)
{
  s << v.ToString();
  return s;
}

void
LoadEnumType(EnumType* type, FileSystem* fs, const std::string& path)
{
  ASSERT(type);

  enumlist::Enumroot root;
  const std::string  load_error = LoadProtoJson(fs, &root, path);

  std::vector<std::string> names;

  if(!load_error.empty())
  {
    std::cerr << "Failed to load enums " << path << ": " << load_error << "\n";
  }
  else
  {
    for(const auto& name : root.name())
    {
      names.push_back(name);
    }
  }

  type->AddEnums(names);
}
