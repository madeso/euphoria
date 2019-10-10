#ifndef EUPHORIA_ID_H
#define EUPHORIA_ID_H

#include <vector>

namespace euphoria::core
{
    struct IdGenerator
    {
    public:
        typedef int ID;

        IdGenerator();

        ID
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

        [[nodiscard]]
        static const Id&
        Invalid();

        ~Id();

        bool
        IsValid() const;

        void
        Generate(IdGenerator* generator);

        IdGenerator::ID
        GetValue() const;

    private:
        IdGenerator::ID value_;
        IdGenerator*    generator_;
    };

}  // namespace euphoria::core

#endif  // EUPHORIA_ID_H
