#include <iostream>
#include "core/enum.h"
#include "core/assert.h"
#include "core/proto.h"

#include "enum.pb.h"

EnumType::EnumType()
  : isAdding_(true)
  , nextIndex_(0)
{
}

EnumType::~EnumType()
{
  Assert(isAdding_ == false);

  if(!createdButNotAddedList.empty())
  {
    std::cout << "EnumType error\n";
  }

  // Assert(createdButNotAddedList.empty() == true);
}

const std::string& EnumType::ToString(size_t v) const
{
  Assert(v < nextIndex_);
  ValueToName::const_iterator f = valueToName_.find(v);
  if(f != valueToName_.end())
  {
    return f->second;
  }
  else
  {
    ValueToName::const_iterator i = createdButNotAddedList.find(v);
    if(i == createdButNotAddedList.end())
    {
      throw "unknown index";
    }
    else
    {
      return i->second;
    }
  }
}

const EnumValue EnumType::ToEnum(const std::string &name)
{
  NameToValue::const_iterator r = nameToValue_.find(name);
  if(r == nameToValue_.end())
  {
    if(isAdding_)
    {
      const size_t id = nextIndex_;
      ++nextIndex_;
      createdButNotAddedList.insert(ValueToName::value_type(id, name));
      createdButNotAddedMap.insert(NameToValue::value_type(name, id));
      nameToValue_.insert(NameToValue::value_type(name, id));
      return EnumValue(this, id);
    }
    else
    {
      throw "loading has finished, enum doesnt exist or is misspelled";
    }
  }
  else
  {
    return EnumValue(this, r->second);
  }
}

void EnumType::AddEnum(const std::string &name)
{
  Assert(isAdding_ == true);
  NameToValue::const_iterator r = nameToValue_.find(name);
  if(r == nameToValue_.end())
  {
    const size_t id = nextIndex_;
    ++nextIndex_;
    valueToName_.insert(ValueToName::value_type(id, name));
    nameToValue_.insert(NameToValue::value_type(name, id));
  }
  else
  {
    NameToValue::const_iterator f = createdButNotAddedMap.find(name);
    if(f == createdButNotAddedMap.end())
    {
      throw "enum already added";
    }
    else
    {
      // move to list
      const size_t id = f->second;
      ValueToName::const_iterator i = createdButNotAddedList.find(id);
      Assert(i != createdButNotAddedList.end()); // createdButNotAdded list/map inconsistencies
      createdButNotAddedList.erase(i);
      createdButNotAddedMap.erase(f);
    }
  }
}

void EnumType::StopAdding()
{
  Assert(isAdding_ == true);
  isAdding_ = false;

  if( !createdButNotAddedList.empty() )
  {
    for(const auto name: createdButNotAddedList)
    {
      std::cout << "Invalid type detected: " << name.second << "\n";
    }

    std::cout << "\n";
    std::cout << "Valid names: \n";
    std::cout << "============\n";

    if(valueToName_.empty())
    {
      std::cout << "Zero valid names detected!!!\n";
    }
    else
    {
      for(const auto name: valueToName_)
      {
        std::cout << " * " << name.second << "\n";
      }
    }
  }

  // if this isn't empty, some enums have not been added or misspelling has occured, see throw above
  // Assert(createdButNotAddedList.empty() == true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enum Value

EnumValue::EnumValue(EnumType* t, size_t v)
  : type_(t)
  , value_(v)
{
}

const std::string EnumValue::ToString() const
{
  return type_->ToString(value_);
}

const size_t EnumValue::ToValue() const
{
  return value_;
}

bool EnumValue::operator==(const EnumValue& other) const
{
  Assert(type_ == other.type_);
  return value_ == other.value_;
}

bool EnumValue::operator!=(const EnumValue& other) const
{
  return !(*this == other);
}

bool EnumValue::operator<(const EnumValue& other) const
{
  Assert(type_ == other.type_);
  return value_ < other.value_;
}

std::ostream& operator<<(std::ostream& s, const EnumValue& v)
{
  s << v.ToString();
  return s;
}

void LoadEnumType(EnumType* type, FileSystem* fs, const std::string& path)
{
  Assert(type);

  enumlist::Enumroot root;
  const std::string load_error = LoadProtoJson(fs, &root, path);
  if(!load_error.empty())
  {
    std::cerr << "Failed to load enums " << path << ": " << load_error << "\n";
  }
  else
  {
    for(const auto name: root.name())
    {
      type->AddEnum(name);
    }
  }

  type->StopAdding();
}
