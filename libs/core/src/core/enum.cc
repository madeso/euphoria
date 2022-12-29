#include "core/enum.h"

#include <iostream>
#include <set>
#include <utility>

#include "assert/assert.h"
#include "core/str.h"
#include "core/stringmerger.h"
#include "core/stringutils.h"
#include "core/proto.h"
#include "log/log.h"
#include "core/vfs_path.h"

#ifdef assert
#undef assert
#endif
#include <cassert>

#include "gaf_enum.h"
#include "gaf_rapidjson_enum.h"

namespace euphoria::core
{
    EnumType::EnumType(std::string name)
        : type_name(std::move(name))
        , is_adding(true)
        , next_index(1)
    {
    }


    EnumType::~EnumType()
    {
        ASSERT(!is_adding);
    }


    std::string
    EnumType::to_string(size_t v) const
    {
        ASSERT(v < next_index);
        auto f = value_to_name.find(v);
        if(f != value_to_name.end())
        {
            return f->second;
        }

        // ASSERT(false && "Invalid index");
        const auto values = string_mergers::english_or.merge
        (
            to_string_vector(name_to_value)
        );
        
        return "<invalid value {} of {}>"_format(v, values);
    }


    EnumValue
    EnumType::to_enum(const std::string& name)
    {
        auto r = name_to_value.find(name);
        if(r != name_to_value.end())
        {
            return EnumValue(this, r->second);
        }

        if(!is_adding)
        {
            LOG_ERROR("Enum value doesnt exist, {0}", name);
            return EnumValue(this, 0);
        }
        const size_t id = next_index;
        add_enum(name);
        return EnumValue(this, id);
    }


    void
    EnumType::add_enums(const std::vector<std::string>& names)
    {
        ASSERT(is_adding);
        std::set<std::string> valid_names;
        for(const auto& name: names)
        {
            add_enum(name);
            valid_names.insert(name);
        }

        // validate all names against true names
        for(const auto& name: name_to_value)
        {
            const bool missing
                    = valid_names.find(name.first) == valid_names.end();
            if(missing)
            {
                LOG_ERROR
                (
                    "Enum {0} was registered with name {1} but that is invalid.",
                    type_name,
                    name.first
                );
            }
        }

        is_adding = false;
    }


    void
    EnumType::add_enum(const std::string& name)
    {
        ASSERT(is_adding);
        auto r = name_to_value.find(name);
        if(r != name_to_value.end())
        {
            return;
        }
        const size_t id = next_index;
        ++next_index;
        value_to_name.insert(ValueToNameMap::value_type(id, name));
        name_to_value.insert(NameToValueMap::value_type(name, id));
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Enum Value

    EnumValue::EnumValue(EnumType* t, size_t v) : type(t), value(v) {}


    std::string
    EnumValue::to_string() const
    {
        return type->to_string(value);
    }


    size_t
    EnumValue::to_value() const
    {
        return value;
    }


    bool
    EnumValue::operator==(const EnumValue& other) const
    {
        ASSERT(type == other.type);
        return value == other.value;
    }


    bool
    EnumValue::operator!=(const EnumValue& other) const
    {
        return !(*this == other);
    }


    bool
    EnumValue::operator<(const EnumValue& other) const
    {
        ASSERT(type == other.type);
        return value < other.value;
    }


    std::ostream&
    operator<<(std::ostream& s, const EnumValue& v)
    {
        s << v.to_string();
        return s;
    }


    void
    load_enum_type
    (
        EnumType* type,
        vfs::FileSystem* fs,
        const vfs::FilePath& path
    )
    {
        ASSERT(type);

        const auto root = get_default_but_log_errors
        (
            read_json_file_to_gaf_struct<enumlist::Enumroot>(fs, path, enumlist::ReadJsonEnumroot)
        );

        // todo(Gustav): handle enum errors better...

        std::vector<std::string> names;
        for(const auto& name: root.name)
        {
            names.push_back(name);
        }
        type->add_enums(names);
    }

}
