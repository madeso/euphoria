#pragma once


namespace eu::window
{
    struct SdlLibrary
    {
    public:
        SdlLibrary();
        ~SdlLibrary();

        SdlLibrary(const SdlLibrary& other) = delete;
        void operator=(const SdlLibrary&) = delete;
        SdlLibrary(SdlLibrary&& other) = delete;
        void operator=(SdlLibrary&&) = delete;

        bool ok;
    };
}
