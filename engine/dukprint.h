#ifndef EUPHORIA_DUKPRINT_H
#define EUPHORIA_DUKPRINT_H

namespace euphoria::duk
{
    class Duk;
}

namespace euphoria::engine
{
    void
    AddPrint(euphoria::duk::Duk* duk);
}

#endif  // EUPHORIA_DUKPRINT_H
