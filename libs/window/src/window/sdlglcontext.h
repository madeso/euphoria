#pragma once


namespace euphoria::window
{
    struct SdlWindow;

    struct SdlAndOpenglContext
    {
    public:
        SdlAndOpenglContext(SdlWindow* window);
        ~SdlAndOpenglContext();

        SdlAndOpenglContext(const SdlAndOpenglContext& other) = delete;
        void operator=(const SdlAndOpenglContext&) = delete;
        SdlAndOpenglContext(SdlAndOpenglContext&& other) = delete;
        void operator=(SdlAndOpenglContext&&) = delete;

        void* context;
    };
}
