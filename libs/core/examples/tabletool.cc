#include <fstream>
#include <sstream>

#include "core/table_string.h"
#include "core/argparse.h"

using namespace euphoria::core;

enum class type
{
    simple,
    grid
};

std::string
stream_to_string(std::istream& out)
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
    type type = type::simple;
    CsvTrim trim = CsvTrim::trim;

    {
        auto parser = argparse::Parser {"csvtool"};

        parser.add("-format", &format).set_help("The CSV format used");
        parser.set_const("-simple", &type, type::simple);
        parser.set_const("-grid", &type, type::grid);
        parser.set_const("-notrim", &trim, CsvTrim::dont_trim);
        parser.add("CSV-file", &file);
        if(const auto r = parser.parse(argc, argv))
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
        table = table_from_csv(stream_to_string(stream), options);
    }

    switch(type)
    {
    case type::simple: print_table_simple(std::cout, table); break;
    case type::grid: print_table_grid(std::cout, table); break;
    }

    return 0;
}
