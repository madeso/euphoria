#pragma once

namespace eu::render
{
    void
    print_all_opengl_errors(const char* file, int line);

    void
    setup_opengl_debug();
}

// #define CheckOpenglError() ::eu::render::print_all_opengl_errors(__FILE__, __LINE__)

