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

    const auto args = euphoria::core::argparse::Args::Extract(argc, argv);

    if(t3d.Start(args) == false)
    {
        return -1;
    }

    t3d.AddLibrary(euphoria::core::vfs::DirPath{"~/world/"});
    t3d.AddLibrary(euphoria::core::vfs::DirPath{"~/FBX/"});

    while(t3d.running)
    {
        t3d.Frame();
    }

    return 0;
}
