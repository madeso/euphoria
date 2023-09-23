#include "core/enum.h"

#include <iostream>
#include <set>
#include <utility>

#include "assert/assert.h"

#include "log/log.h"

#include "base/stringmerger.h"
#include "base/stringutils.h"

#include "files/enum.h"
#include "io/vfs_path.h"


#ifdef assert
#undef assert
#endif



namespace eu::core
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
        
        return fmt::format("<invalid value {} of {}>", v, values);
    }


    EnumValue
    EnumType::to_enum(const std::string& name)
    {
        auto r = name_to_value.find(name);
        if(r != name_to_value.end())
        {
            return {this, r->second};
        }

        if(!is_adding)
        {
            LOG_ERROR("Enum value doesnt exist, {0}", name);
            return {this, 0};
        }
        const size_t id = next_index;
        add_enum(name);
        return {this, id};
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

    EnumValue::EnumValue(EnumType* t, size_t v)
        : type(t)
        , value(v)
    {
    }


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


    std::string to_string(const EnumValue& v)
    {
        return v.to_string();
    }


    void
    load_enum_type
    (
        EnumType* type,
        io::FileSystem* fs,
        const io::FilePath& path
    )
    {
        ASSERT(type);

        const auto loaded = read_json_file(fs, path);
        if (!loaded)
        {
            // todo(Gustav): handle enum errors better...
            LOG_ERROR("failed to read {}: {}", path, loaded.get_error().display);
            return;
        }

        files::enums::Root root;
        const auto& json = loaded.get_value();

        if (false == files::enums::parse(log::get_global_logger(), &root, json.root, &json.doc))
        {
            return;
        }

        type->add_enums(root.names);
    }

}
