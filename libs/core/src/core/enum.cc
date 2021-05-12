#include "core/enum.h"

#include <iostream>
#include <set>
#include <utility>

#include "core/assert.h"
#include "core/str.h"
#include "core/stringmerger.h"
#include "core/stringutils.h"
#include "core/proto.h"
#include "core/log.h"
#include "core/vfs_path.h"

#ifdef assert
#undef assert
#endif
#include <cassert>

#include "gaf_enum.h"

namespace euphoria::core
{
    LOG_SPECIFY_DEFAULT_LOGGER("core.enum")


    enum_type::enum_type(std::string name)
        : type_name(std::move(name)), is_adding(true), next_index(1)
    {}


    enum_type::~enum_type()
    {
        ASSERT(!is_adding);
    }


    std::string
    enum_type::to_string(size_t v) const
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
        const std::string invalid = string_builder() << "<invalid value " << v << " of "
                                          << values << ">";
        return invalid;
    }


    enum_value
    enum_type::to_enum(const std::string& name)
    {
        auto r = name_to_value.find(name);
        if(r != name_to_value.end())
        {
            return enum_value(this, r->second);
        }

        if(!is_adding)
        {
            LOG_ERROR("Enum value doesnt exist, {0}", name);
            return enum_value(this, 0);
        }
        const size_t id = next_index;
        add_enum(name);
        return enum_value(this, id);
    }


    void
    enum_type::add_enums(const std::vector<std::string>& names)
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
    enum_type::add_enum(const std::string& name)
    {
        ASSERT(is_adding);
        auto r = name_to_value.find(name);
        if(r != name_to_value.end())
        {
            return;
        }
        const size_t id = next_index;
        ++next_index;
        value_to_name.insert(value_to_name_map::value_type(id, name));
        name_to_value.insert(name_to_value_map::value_type(name, id));
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Enum Value

    enum_value::enum_value(enum_type* t, size_t v) : type(t), value(v) {}


    std::string
    enum_value::to_string() const
    {
        return type->to_string(value);
    }


    size_t
    enum_value::to_value() const
    {
        return value;
    }


    bool
    enum_value::operator==(const enum_value& other) const
    {
        ASSERT(type == other.type);
        return value == other.value;
    }


    bool
    enum_value::operator!=(const enum_value& other) const
    {
        return !(*this == other);
    }


    bool
    enum_value::operator<(const enum_value& other) const
    {
        ASSERT(type == other.type);
        return value < other.value;
    }


    std::ostream&
    operator<<(std::ostream& s, const enum_value& v)
    {
        s << v.to_string();
        return s;
    }


    void
    load_enum_type
    (
        enum_type* type,
        vfs::file_system* fs,
        const vfs::file_path& path
    )
    {
        ASSERT(type);

        enumlist::Enumroot root;
        const std::string  load_error = read_json_to_gaf_struct_or_get_error_message(fs, &root, path);

        std::vector<std::string> names;

        if(!load_error.empty())
        {
            LOG_ERROR("Failed to load enums {0}: {1}", path, load_error);
        }
        else
        {
            for(const auto& name: root.name)
            {
                names.push_back(name);
            }
        }

        type->add_enums(names);
    }

}  // namespace euphoria::core
