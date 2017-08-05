#ifndef EUPHORIA_ENUM_H
#define EUPHORIA_ENUM_H

#include <string>
#include <map>

class EnumValue;

// todo: as a compile option, use a hash instead of the string enum

/// Represents a enum type. Declare globally grab specific enums before or after load, and load adds new, verifies created and asserts misspelled values.
// Advantages over hashes: type safety, catches bad spelling, no collisions and  perhaps faster to generate?
class EnumType
{
 public:
  EnumType();
  ~EnumType();

  const std::string& ToString(size_t v) const;
  const EnumValue ToEnum(const std::string &name);
  void AddEnum(const std::string &name);
  void StopAdding();
 private:
  typedef std::map<size_t, std::string> ValueToName;
  typedef std::map<std::string, size_t> NameToValue;

  ValueToName valueToName_;
  NameToValue nameToValue_;

  ValueToName createdButNotAddedList;
  NameToValue createdButNotAddedMap;

  bool isAdding_;
  size_t nextIndex_;
};

class EnumValue
{
 public:
  EnumValue(EnumType* type, size_t value);

  // todo: add EnumType to the parameter to verify against stored member
  // so
  const std::string ToString() const;
  const size_t ToValue() const;

  bool operator==(const EnumValue& other) const;
  bool operator!=(const EnumValue& other) const;
  bool operator<(const EnumValue& other) const;
 private:
  // todo: only have type_ in debug/test builds
  EnumType* type_;
  size_t value_;
};

std::ostream& operator<<(std::ostream& s, const EnumValue& v);


#endif //EUPHORIA_ENUM_H
