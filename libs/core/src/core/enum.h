#pragma once


#include <map>


namespace eu::core
{
    namespace vfs
    {
        struct FileSystem;
        struct FilePath;
    }

    struct EnumValue;

    // todo(Gustav): as a compile option, use a hash instead of the string enum
    // todo(Gustav): rename to something like DynEnum to not be confused with regular enums

    /** Represents a dynamically loaded "enum type".
     * Declare globally grab specific enums before or after load, and load adds new,
     * verifies created and asserts misspelled values.
     * Advantages over hashes: type safety, catches bad spelling, no collisions and
     * perhaps faster to generate?
     */
    struct EnumType
    {
        using ValueToNameMap = std::map<size_t, std::string>;
        using NameToValueMap = std::map<std::string, size_t>;

        std::string type_name;
        ValueToNameMap value_to_name;
        NameToValueMap name_to_value;
        bool is_adding;
        size_t next_index;

        EnumType(std::string name);
        ~EnumType();

        EnumType(const EnumType&) = delete;
        EnumType(EnumType&&) = delete;
        void operator=(const EnumType&) = delete;
        void operator=(EnumType&&) = delete;

        [[nodiscard]] EnumValue to_enum(const std::string& name);
        void add_enums(const std::vector<std::string>& names);
        void add_enum(const std::string& name);

        [[nodiscard]] std::string to_string(size_t v) const;
    };

    struct EnumValue
    {
        // todo(Gustav): only have the type in debug/test builds
        EnumType* type;
        size_t value;

        EnumValue(EnumType* t, size_t v);

        // todo(Gustav): add enum_type to the parameter to verify against stored member
        [[nodiscard]] std::string to_string() const;
        [[nodiscard]] size_t to_value() const;

        bool operator==(const EnumValue& other) const;
        bool operator!=(const EnumValue& other) const;
        bool operator<(const EnumValue& other) const;
    };


    std::string to_string(const EnumValue& v);


    void
    load_enum_type
    (
        EnumType* type,
        vfs::FileSystem* fs,
        const vfs::FilePath& path
    );


#define DECLARE_ENUM_TYPE(NAME) EnumType& NAME##_EnumType();
#define IMPLEMENT_ENUM_TYPE(NAME) \
    EnumType& NAME##_EnumType() \
    { \
        static EnumType type {#NAME}; \
        return type; \
    }


// std::string constructor may throw
// todo(Gustav): provide compile time option to use hashes instead
// http://aras-p.info/blog/2016/08/09/More-Hash-Function-Tests/
#define DEFINE_ENUM_VALUE(TYPE, NAME, STRING) \
    const ::eu::core::EnumValue NAME = TYPE##_EnumType().to_enum(STRING)
#define SET_ENUM_FROM_FILE(FS, PATH, TYPE) \
    load_enum_type(&TYPE##_EnumType(), FS, PATH)

}
ADD_DEFAULT_FORMATTER(eu::core::EnumValue, std::string, eu::core::to_string);
