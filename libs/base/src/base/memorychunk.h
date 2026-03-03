namespace eu
{

struct MemoryChunk
{
    std::vector<char> bytes;
};

MemoryChunk chunk_from_file(const std::string& path);

}
