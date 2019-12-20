#include "core/argparse.h"
#include "core/log.h"

#include "t3d/t3d.h"
#include "t3d/tilelibrary.h"

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

    t3d.AddLibrary("world");

    if(t3d.tile_library->tiles.empty())
    {
        LOG_ERROR("No tile loaded!");
        return -2;
    }

    while(t3d.running)
    {
        t3d.Frame();
    }

    return 0;
}
