#include "core/argparse.h"
#include "log/log.h"

#include "t3d/t3d.h"
#include "t3d/tilelibrary.h"

#include "SDL.h"

int
main(int argc, char** argv)
{
    eu::t3d::Application t3d;

    const auto args = eu::core::argparse::NameAndArguments::extract(argc, argv);

    if(const auto r = t3d.start(args); r != 0)
    {
        return r;
    }

    t3d.add_library(eu::core::vfs::DirPath{"~/world/"});
    t3d.add_library(eu::core::vfs::DirPath{"~/FBX/"});

    while(t3d.running)
    {
        t3d.on_frame();
    }

    return 0;
}
