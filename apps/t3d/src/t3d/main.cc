#include "core/argparse.h"
#include "core/log.h"

#include "t3d/t3d.h"
#include "t3d/tilelibrary.h"

#include "SDL.h"

LOG_SPECIFY_DEFAULT_LOGGER("t3d")

int
main(int argc, char** argv)
{
    euphoria::t3d::application t3d;

    const auto args = euphoria::core::argparse::name_and_arguments::extract(argc, argv);

    if(const auto r = t3d.start(args); r != 0)
    {
        return r;
    }

    t3d.add_library(euphoria::core::vfs::dir_path{"~/world/"});
    t3d.add_library(euphoria::core::vfs::dir_path{"~/FBX/"});

    while(t3d.running)
    {
        t3d.on_frame();
    }

    return 0;
}
