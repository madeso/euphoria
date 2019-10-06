#include "core/base64.h"

#include <iostream>
#include <string>

#include "core/vfs.h"

namespace vfs = euphoria::core::vfs;

void
PrintUsage(char** argv)
{
    std::cout << "USAGE:\n"
              << argv[0] << " encode FILE\n"
              << argv[0] << " decode STRING\n"
              << "\n";
}

int
RunEncode(const std::string& data)
{
    vfs::FileSystem file_system;
    auto            catalog = vfs::FileSystemRootCatalog::AddRoot(&file_system);
    vfs::FileSystemRootFolder::AddRoot(&file_system);

    auto memory  = file_system.ReadFile(data);
    auto encoded = euphoria::core::base64::Encode(memory);
    std::cout << "Encoded:\n" << encoded << "\n";
    return 0;
}

int
RunDecode(const std::string& data)
{
    std::cerr << "Not implemented yet...\n";
    return -100;
}

int
main(int argc, char** argv)
{
    if (argc == 3)
    {
        const std::string command = argv[1];
        const std::string data    = argv[2];

        if (command == "encode")
        {
            return RunEncode(data);
        }
        if (command == "decode")
        {
            return RunDecode(data);
        }

        PrintUsage(argv);
        return -2;
    }
    else
    {
        PrintUsage(argv);
        return -1;
    }
}
