#include "core/base64.h"

#include <iostream>


#include "core/vfs.h"
#include "core/vfs_path.h"


namespace vfs = eu::core::vfs;


void
print_usage(char** argv)
{
    std::cout << "USAGE:\n"
              << argv[0] << " encode FILE\n"
              << argv[0] << " decode STRING\n"
              << "\n";
}

int
run_encode(const std::string& data)
{
    vfs::FileSystem file_system;
    auto catalog = vfs::ReadRootCatalog::create_and_add(&file_system);
    vfs::ReadRootPhysicalFolder::add_current_directory(&file_system);

    auto memory = file_system.read_file
    (
        vfs::FilePath::from_script(data).value_or
        (
            vfs::FilePath{"~/invalid_input"}
        )
    );
    auto encoded = eu::core::base64::encode(memory);
    std::cout << "Encoded:\n" << encoded << "\n";
    return 0;
}

int
run_decode(const std::string&)
{
    std::cerr << "Not implemented yet...\n";
    return -100;
}

int
main(int argc, char** argv)
{
    // todo(Gustav): use the argparse instead...
    if(argc == 3)
    {
        const std::string command = argv[1];
        const std::string data = argv[2];

        if(command == "encode")
        {
            return run_encode(data);
        }
        if(command == "decode")
        {
            return run_decode(data);
        }

        print_usage(argv);
        return -2;
    }
    else
    {
        print_usage(argv);
        return -1;
    }
}
