#pragma once

#include <vector>

#include "core/noncopyable.h"


namespace euphoria::core
{
    // todo(Gustav): refactor into a better design

    struct IdGenerator
    {
        using Id = int;

        IdGenerator();

        [[nodiscard]] Id
        generate();

        void
        release(Id id);

    private:
        Id current;
        std::vector<Id> released;
    };


    struct Id
    {
    private:
        Id();

    public:
        explicit Id(IdGenerator* generator);

        NONCOPYABLE(Id);

        [[nodiscard]] static const Id&
        invalid_value();

        ~Id();

        [[nodiscard]] bool
        is_valid() const;

        void
        generate(IdGenerator* generator);

        [[nodiscard]] IdGenerator::Id
        get_value() const;

    private:
        IdGenerator::Id value;
        IdGenerator* generator;
    };

}
