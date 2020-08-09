#ifndef EUPHORIA_DUK_BINDFUNCTION_H
#define EUPHORIA_DUK_BINDFUNCTION_H

#include <string>

namespace euphoria::duk
{
    struct Context;

    struct Function
    {
    public:
        Function() = default;

        virtual ~Function() = default;

        // return empty string if matches
        // non-empty means match failed
        virtual std::string
        Matches(Context* ctx)
                = 0;

        virtual int
        Call(Context* ctx)
                = 0;

        virtual std::string
        Describe(Context* context) const = 0;
    };
}  // namespace euphoria::duk

#endif  // EUPHORIA_DUK_BINDFUNCTION_H
