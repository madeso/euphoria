#ifndef EUPHORIA_ID_H
#define EUPHORIA_ID_H

#include <vector>

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

        Id(const Id&) = delete;
        Id(Id&&) = delete;
        void operator=(const Id&) = delete;
        void operator=(Id&&) = delete;

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

}  // namespace euphoria::core

#endif  // EUPHORIA_ID_H
