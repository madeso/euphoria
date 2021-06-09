#include "core/id.h"

#include "core/assert.h"

namespace euphoria::core
{
    id_generator::id_generator() : current_(1) {}

    id_generator::id
    id_generator::generate()
    {
        if(released_.empty())
        {
            const id value = current_;
            ++current_;
            return value;
        }
        else
        {
            const id value = *released_.rbegin();
            released_.pop_back();
            return value;
        }
    }

    void
    id_generator::release(id_generator::id id)
    {
        released_.push_back(id);
    }

    ////////////////////////////////////////////////////////////////////////////////

    id::id() : value_(0), generator_(nullptr) {}

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
        : value_(generate_id(generator)), generator_(generator)
    {
    }

    const id&
    id::Invalid()
    {
        static id InvalidValue;
        ASSERT(!InvalidValue.is_valid());
        return InvalidValue;
    }

    id::~id()
    {
        generator_->release(value_);
    }

    bool
    id::is_valid() const
    {
        return value_ > 0 && generator_ != nullptr;
    }

    void
    id::generate(id_generator* generator)
    {
        ASSERT(!is_valid());
        ASSERT(generator);
        value_ = generator->generate();
        generator_ = generator;
    }

    id_generator::id
    id::get_value() const
    {
        ASSERT(is_valid());
        return value_;
    }

}
