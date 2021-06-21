#include "core/id.h"

#include "core/assert.h"

namespace euphoria::core
{
    id_generator::id_generator() : current(1) {}

    id_generator::id
    id_generator::generate()
    {
        if(released.empty())
        {
            const id value = current;
            ++current;
            return value;
        }
        else
        {
            const id value = *released.rbegin();
            released.pop_back();
            return value;
        }
    }

    void
    id_generator::release(id_generator::id id)
    {
        released.push_back(id);
    }

    ////////////////////////////////////////////////////////////////////////////////

    id::id() : value(0), generator(nullptr) {}

    namespace
    {
        id_generator::id
        generate_id(id_generator* generator)
        {
            ASSERT(generator);
            return generator->generate();
        }
    }

    id::id(id_generator* generator)
        : value(generate_id(generator)), generator(generator)
    {
    }

    const id&
    id::invalid_value()
    {
        static id the_invalid_value;
        ASSERT(!the_invalid_value.is_valid());
        return the_invalid_value;
    }

    id::~id()
    {
        generator->release(value);
    }

    bool
    id::is_valid() const
    {
        return value > 0 && generator != nullptr;
    }

    void
    id::generate(id_generator* generator)
    {
        ASSERT(!is_valid());
        ASSERT(generator);
        value = generator->generate();
        generator = generator;
    }

    id_generator::id
    id::get_value() const
    {
        ASSERT(is_valid());
        return value;
    }

}
