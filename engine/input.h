#ifndef EUPHORIA_INPUT_H
#define EUPHORIA_INPUT_H

#include <string>
#include <memory>
#include <vector>

#include "core/key.h"

#include "duk/objectreference.h"

namespace euphoria::duk
{
    class Duk;
}

namespace euphoria::engine
{
    struct BoundVar
    {
        std::string name;
        float       state;
        float       last_state;
        core::Key   key;

        BoundVar(const std::string& n, const core::Key& k);

        // todo: figure out how to bind keys to this property
        // todo: figure out how to handle just pressed
        // todo: figure out how to handle player characters
        // todo: figure out how to handle inputs other than keyboard
    };

    class Input
    {
        public:
        static void
        Bind(duk::Duk* duk);

        void
        Add(std::shared_ptr<BoundVar> bind);

        void
        SetKeyState(core::Key key, float state);

        void
        Set(duk::Duk* duk, duk::ObjectReference container) const;

        void
        UpdateState();

        private:
        std::vector<std::shared_ptr<BoundVar>> binds;
    };
}  // namespace euphoria::engine

TYPEID_SETUP_TYPE(euphoria::engine::BoundVar);

#endif  // EUPHORIA_INPUT_H
