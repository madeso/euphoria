/** \def ENABLE_HIGH_PERFORMANCE_GRAPHICS
    \brief This tries to enable the "high performance" graphics mode for a hybrid graphics setup.

    \note
    This macro only applies to windows.

## Credits:
    - Graphics Programming discord
    - https://gist.github.com/statico/6809850727c708f08458
    - https://www.reddit.com/r/gamedev/comments/bk7xbe/psa_for_anyone_developing_a_gameengine_in_c/

## Additional resources:
    - http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
    - http://developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
*/

#ifdef _WIN32
    #define ENABLE_HIGH_PERFORMANCE_GRAPHICS \
    extern "C" \
    { \
        __declspec(dllexport) unsigned long NvOptimusEnablement = 1; \
        __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1; \
    }

#else
    #define ENABLE_HIGH_PERFORMANCE_GRAPHICS
#endif
