#include "core/argparse.h"
#include "core/log.h"

#include "t3d/t3d.h"
#include "t3d/tilelibrary.h"

#include "SDL.h"

LOG_SPECIFY_DEFAULT_LOGGER("t3d")

int
main(int argc, char** argv)
{
    euphoria::t3d::T3d t3d;

    const auto args = euphoria::core::argparse::Arguments::Extract(argc, argv);

    if(const auto r = t3d.Start(args); r != 0)
    {
        return r;
    }

    t3d.AddLibrary(euphoria::core::vfs::DirPath{"~/world/"});
    t3d.AddLibrary(euphoria::core::vfs::DirPath{"~/FBX/"});

    while(t3d.running)
    {
        t3d.Frame();
    }

    return 0;
}
