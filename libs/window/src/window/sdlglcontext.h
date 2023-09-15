#pragma once


namespace eu::window
{
    struct SdlWindow;

    struct SdlOpenglContext
    {
    public:
        SdlOpenglContext(SdlWindow* window);
        ~SdlOpenglContext();

        SdlOpenglContext(const SdlOpenglContext& other) = delete;
        void operator=(const SdlOpenglContext&) = delete;
        SdlOpenglContext(SdlOpenglContext&& other) = delete;
        void operator=(SdlOpenglContext&&) = delete;

        void* context;
    };
}
