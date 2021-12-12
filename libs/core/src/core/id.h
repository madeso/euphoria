#pragma once

#include <vector>

#include "core/noncopyable.h"


namespace euphoria::core
{
    // todo(Gustav): refactor into a better design

    struct IdGenerator
    {
        using id = int;

        IdGenerator();

        [[nodiscard]] id
        generate();

        void
        release(id id);

    private:
        id current;
        std::vector<id> released;
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

        [[nodiscard]] IdGenerator::id
        get_value() const;

    private:
        IdGenerator::id value;
        IdGenerator* generator;
    };

}
