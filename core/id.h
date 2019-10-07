#ifndef EUPHORIA_ID_H
#define EUPHORIA_ID_H

#include <vector>
#include "core/ints.h"

namespace euphoria::core
{
    class IdGenerator
    {
    public:
        typedef uint32 ID;

        IdGenerator();

        const ID
        Generate();

        void
        Release(ID id);

    private:
        ID              current_;
        std::vector<ID> released_;
    };

    class Id
    {
    private:
        Id();

    public:
        explicit Id(IdGenerator* generator);

        static const Id&
        Invalid();

        ~Id();

        bool
        IsValid() const;

        void
        Generate(IdGenerator* generator);

        const IdGenerator::ID
        GetValue() const;

    private:
        IdGenerator::ID value_;
        IdGenerator*    generator_;
    };

}  // namespace euphoria::core

#endif  // EUPHORIA_ID_H
