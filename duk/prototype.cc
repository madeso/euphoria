#include "duk/prototype.h"

Prototype::Prototype(const std::string& n, void* p)
    : name(n)
    , prototype(p)
{
}
