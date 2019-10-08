#ifndef EUPHORIA_ENUM_H
#define EUPHORIA_ENUM_H

#include <string>
#include <map>
#include <vector>

namespace euphoria::core
{
    namespace vfs
    {
        class FileSystem;
    }

    class EnumValue;

    // todo: as a compile option, use a hash instead of the string enum

    /// Represents a enum type. Declare globally grab specific enums before or after
    /// load, and load adds new,
    /// verifies created and asserts misspelled values.
    // Advantages over hashes: type safety, catches bad spelling, no collisions and
    // perhaps faster to generate?
    class EnumType
    {
    public:
        EnumType(std::string name);
        ~EnumType();

        std::string
        ToString(size_t v) const;
        const EnumValue
        ToEnum(const std::string& name);

        void
        AddEnums(const std::vector<std::string>& names);

    private:
        typedef std::map<size_t, std::string> ValueToName;
        typedef std::map<std::string, size_t> NameToValue;

        void
        AddEnum(const std::string& name);

        std::string name_;

        ValueToName valueToName_;
        NameToValue nameToValue_;

        bool   isAdding_;
        size_t nextIndex_;
    };

    class EnumValue
    {
    public:
        EnumValue(EnumType* t, size_t v);

        // todo: add EnumType to the parameter to verify against stored member
        // so
        const std::string
        ToString() const;
        const size_t
        ToValue() const;

        bool
        operator==(const EnumValue& other) const;
        bool
        operator!=(const EnumValue& other) const;
        bool
        operator<(const EnumValue& other) const;

    private:
        // todo: only have type_ in debug/test builds
        EnumType* type_;
        size_t    value_;
    };

    std::ostream&
    operator<<(std::ostream& s, const EnumValue& v);

    void
    LoadEnumType(EnumType* type, vfs::FileSystem* fs, const std::string& path);

#define DECLARE_ENUM_TYPE(NAME) EnumType& NAME##_EnumType();
#define IMPLEMENT_ENUM_TYPE(NAME)                                              \
    EnumType& NAME##_EnumType()                                                \
    {                                                                          \
        static EnumType type {#NAME};                                          \
        return type;                                                           \
    }
#define SET_ENUM_VALUES(NAME, FUNC)                                            \
    do                                                                         \
    {                                                                          \
        FUNC(NAME##_EnumType());                                               \
        NAME##_EnumType().StopAdding();                                        \
    } while(false)

// std::string constructor may throw
// todo: provide compile time option to use hashes instead
// http://aras-p.info/blog/2016/08/09/More-Hash-Function-Tests/
#define DEFINE_ENUM_VALUE(TYPE, NAME, STRING)                                  \
    const ::euphoria::core::EnumValue NAME = TYPE##_EnumType().ToEnum(STRING)
#define SET_ENUM_FROM_FILE(FS, PATH, TYPE)                                     \
    LoadEnumType(&TYPE##_EnumType(), FS, PATH)

}  // namespace euphoria::core

#endif  // EUPHORIA_ENUM_H
