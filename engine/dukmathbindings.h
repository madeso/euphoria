#ifndef EUPHORIA_DUKMATHBINDINGS_H
#define EUPHORIA_DUKMATHBINDINGS_H

namespace euphoria::duk
{
    class Duk;
}

namespace euphoria::engine
{
    void
    BindMath(duk::Duk* duk);
}

#endif  // EUPHORIA_DUKMATHBINDINGS_H
