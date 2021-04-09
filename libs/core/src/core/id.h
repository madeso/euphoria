#pragma once

#include <vector>

#include "core/noncopyable.h"


namespace euphoria::core
{
    // todo(Gustav): refactor into a better design

    struct IdGenerator
    {
        using ID = int;

        IdGenerator();

        [[nodiscard]] ID
        Generate();

        void
        Release(ID id);

    private:
        ID              current_;
        std::vector<ID> released_;
    };

    struct Id
    {
    private:
        Id();

    public:
        explicit Id(IdGenerator* generator);

        NONCOPYABLE(Id);

        [[nodiscard]] static const Id&
        Invalid();

        ~Id();

        [[nodiscard]] bool
        IsValid() const;

        void
        Generate(IdGenerator* generator);

        [[nodiscard]] IdGenerator::ID
        GetValue() const;

    private:
        IdGenerator::ID value_;
        IdGenerator*    generator_;
    };

}
