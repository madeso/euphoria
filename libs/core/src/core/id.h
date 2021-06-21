#pragma once

#include <vector>

#include "core/noncopyable.h"


namespace euphoria::core
{
    // todo(Gustav): refactor into a better design

    struct id_generator
    {
        using id = int;

        id_generator();

        [[nodiscard]] id
        generate();

        void
        release(id id);

    private:
        id current;
        std::vector<id> released;
    };


    struct id
    {
    private:
        id();

    public:
        explicit id(id_generator* generator);

        NONCOPYABLE(id);

        [[nodiscard]] static const id&
        invalid_value();

        ~id();

        [[nodiscard]] bool
        is_valid() const;

        void
        generate(id_generator* generator);

        [[nodiscard]] id_generator::id
        get_value() const;

    private:
        id_generator::id value;
        id_generator* generator;
    };

}
