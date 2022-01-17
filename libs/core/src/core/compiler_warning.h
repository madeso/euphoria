#pragma once


// https://www.fluentcpp.com/2019/08/30/how-to-disable-a-warning-in-cpp/

#if defined(_MSC_VER)
    #define DISABLE_WARNING_PUSH __pragma(warning( push ))
    #define DISABLE_WARNING_POP __pragma(warning( pop ))

    #define DISABLE_WARNING(warningNumber) __pragma(warning( disable : warningNumber ))

    // warning C4459: declaration of 'X' hides global declaration
    // https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4459?view=msvc-170
    #define DISABLE_GLOBAL_DECLARATION    DISABLE_WARNING(4459)
    #define DISABLE_WARNING_UNUSED_VARIABLE
#elif defined(__GNUC__) || defined(__clang__)
    #define DISABLE_WARNING_PUSH DO_PRAGMA(GCC diagnostic push)
    #define DISABLE_WARNING_POP DO_PRAGMA(GCC diagnostic pop)

    #define DO_PRAGMA(X) _Pragma(#X)
    #define DISABLE_WARNING(warningName) DO_PRAGMA(GCC diagnostic ignored #warningName)

    // gcc and clang definitons
    #define DISABLE_GLOBAL_DECLARATION
    #define DISABLE_WARNING_UNUSED_VARIABLE DISABLE_WARNING(-Wunused-variable)
    
    #if !defined(__clang__)
        // gcc definitions
        // #define DISABLE_WARNING_UNUSED_VARIABLE
    #else
        // clang definitions
    #endif
#else
    #define DISABLE_WARNING_PUSH
    #define DISABLE_WARNING_POP

    #define DISABLE_GLOBAL_DECLARATION
    #define DISABLE_WARNING_UNUSED_VARIABLE
#endif

