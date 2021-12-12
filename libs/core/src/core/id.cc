#include "core/id.h"

#include "core/assert.h"

namespace euphoria::core
{
    IdGenerator::IdGenerator() : current(1) {}

    IdGenerator::id
    IdGenerator::generate()
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
    IdGenerator::release(IdGenerator::id id)
    {
        released.push_back(id);
    }

    ////////////////////////////////////////////////////////////////////////////////

    Id::Id() : value(0), generator(nullptr) {}

    namespace
    {
        IdGenerator::id
        generate_id(IdGenerator* generator)
        {
            ASSERT(generator);
            return generator->generate();
        }
    }

    Id::Id(IdGenerator* generator)
        : value(generate_id(generator)), generator(generator)
    {
    }

    const Id&
    Id::invalid_value()
    {
        static Id the_invalid_value;
        ASSERT(!the_invalid_value.is_valid());
        return the_invalid_value;
    }

    Id::~Id()
    {
        generator->release(value);
    }

    bool
    Id::is_valid() const
    {
        return value > 0 && generator != nullptr;
    }

    void
    Id::generate(IdGenerator* the_generator)
    {
        ASSERT(!is_valid());
        ASSERT(the_generator);
        value = the_generator->generate();
        generator = the_generator;
    }

    IdGenerator::id
    Id::get_value() const
    {
        ASSERT(is_valid());
        return value;
    }

}
