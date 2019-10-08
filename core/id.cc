#include "core/id.h"

#include "core/assert.h"

namespace euphoria::core
{
    IdGenerator::IdGenerator() : current_(1) {}

    const IdGenerator::ID
    IdGenerator::Generate()
    {
        if(released_.empty())
        {
            const ID value = current_;
            ++current_;
            return value;
        }
        else
        {
            const ID value = *released_.rbegin();
            released_.pop_back();
            return value;
        }
    }

    void
    IdGenerator::Release(IdGenerator::ID id)
    {
        released_.push_back(id);
    }

    ////////////////////////////////////////////////////////////////////////////////

    Id::Id() : value_(0), generator_(nullptr) {}

    namespace
    {
        IdGenerator::ID
        GenerateId(IdGenerator* generator)
        {
            ASSERT(generator);
            return generator->Generate();
        }
    }  // namespace

    Id::Id(IdGenerator* generator)
        : value_(GenerateId(generator)), generator_(generator)
    {}

    const Id&
    Id::Invalid()
    {
        static Id InvalidValue;
        ASSERT(!InvalidValue.IsValid());
        return InvalidValue;
    }

    Id::~Id()
    {
        generator_->Release(value_);
    }

    bool
    Id::IsValid() const
    {
        return value_ > 0 && generator_ != nullptr;
    }

    void
    Id::Generate(IdGenerator* generator)
    {
        ASSERT(!IsValid());
        ASSERT(generator);
        value_     = generator->Generate();
        generator_ = generator;
    }

    const IdGenerator::ID
    Id::GetValue() const
    {
        ASSERT(IsValid());
        return value_;
    }

}  // namespace euphoria::core
