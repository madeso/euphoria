#include <fstream>
#include <sstream>

#include "core/stringtable.h"
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
    std::vector<std::string> files;
    std::string              format = ",\"";
    Type                     type   = Type::Simple;
    CsvTrim                  trim   = CsvTrim::Trim;

    {
        auto parser = argparse::Parser {"csvtool"};

        parser.AddSimple("-format", &format).Help("The CSV format used");
        parser.AddSimpleFunction("-simple", [&type]() { type = Type::Simple; });
        parser.AddSimpleFunction(
                "-notrim", [&trim]() { trim = CsvTrim::DontTrim; });
        parser.AddSimpleFunction("-grid", [&type]() { type = Type::Grid; });
        parser.AddVector("files", &files).MetaVar("CSV-file");
        auto r = parser.Parse(argc, argv);
        if(r != argparse::ParseResult::Ok)
        {
            return -1;
        }
    }

    int ret = 0;

    bool first = true;
    for(const auto& f: files)
    {
        if(first)
        {
            first = false;
        }
        else
        {
            std::cout << "\n\n\n";
        }

        Table<std::string> table;
        {
            std::ifstream stream {f};
            if(!stream)
            {
                std::cerr << "Failed to load " << f << "\n";
                ret = -1;
            }
            CsvParserOptions options;
            options.delim = format[0];
            options.str   = format[1];
            options.trim  = trim;
            table         = TableFromCsv(StreamToString(stream), options);
        }

        switch(type)
        {
        case Type::Simple: PrintTableSimple(std::cout, table); break;
        case Type::Grid: PrintTableGrid(std::cout, table); break;
        }
    }
    return ret;
}
