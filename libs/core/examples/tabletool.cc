#include <fstream>
#include <sstream>

#include "core/table_string.h"
#include "core/argparse.h"

using namespace euphoria::core;

enum class Type
{
    Simple,
    Grid
};

std::string
StreamToString(std::istream& out)
{
    std::stringstream ss;
    ss << out.rdbuf();
    return ss.str();
}

int
main(int argc, char* argv[])
{
    std::string file;
    std::string format = ",\"";
    Type type = Type::Simple;
    CsvTrim trim = CsvTrim::Trim;

    {
        auto parser = argparse::Parser {"csvtool"};

        parser.Add("-format", &format).Help("The CSV format used");
        parser.SetConst("-simple", &type, Type::Simple);
        parser.SetConst("-grid", &type, Type::Grid);
        parser.SetConst("-notrim", &trim, CsvTrim::DontTrim);
        parser.Add("CSV-file", &file);
        if(const auto r = parser.Parse(argc, argv))
        {
            return *r;
        }
    }

    
    Table<std::string> table;
    {
        std::ifstream stream {file};
        if(!stream)
        {
            std::cerr << "Failed to load " << file << "\n";
            return -1;
        }
        CsvParserOptions options;
        options.delim = format[0];
        options.str = format[1];
        options.trim = trim;
        table = TableFromCsv(StreamToString(stream), options);
    }

    switch(type)
    {
    case Type::Simple: PrintTableSimple(std::cout, table); break;
    case Type::Grid: PrintTableGrid(std::cout, table); break;
    }

    return 0;
}
