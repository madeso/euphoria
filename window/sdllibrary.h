#ifndef EUPHORIA_SDLLIBRARY_H
#define EUPHORIA_SDLLIBRARY_H

namespace euphoria::window
{
    class SdlLibrary
    {
    public:
        SdlLibrary();
        ~SdlLibrary();

        bool ok;
    };
}  // namespace euphoria::window

#endif  // EUPHORIA_SDLLIBRARY_H
