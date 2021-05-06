#ifndef EUPHORIA_INPUT_H
#define EUPHORIA_INPUT_H

#include <string>
#include <memory>
#include <vector>
#include <core/typeinfo.h>

#include "core/key.h"

#include "core/sol.h"


namespace euphoria::engine
{
    struct BoundVar
    {
        std::string name;
        float       state;
        float       last_state;
        core::key   key;

        BoundVar(const std::string& n, const core::key& k);

        // todo(Gustav): figure out how to bind keys to this property
        // todo(Gustav): figure out how to handle just pressed
        // todo(Gustav): figure out how to handle player characters
        // todo(Gustav): figure out how to handle inputs other than keyboard
    };
    
    

    struct Input
    {
    public:
        static void
        Bind(Sol* duk);

        void
        Add(std::shared_ptr<BoundVar> bind);

        void
        SetKeyState(core::key key, float state);

        void
        Set(sol::table* container) const;

        void
        UpdateState();

    private:
        std::vector<std::shared_ptr<BoundVar>> binds;
    };
}  // namespace euphoria::engine

TYPEID_SETUP_TYPE(euphoria::engine::BoundVar);

#endif  // EUPHORIA_INPUT_H
