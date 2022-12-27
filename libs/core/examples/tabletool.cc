#include "core/table_string.h"
#include "core/argparse.h"
#include "core/io.h"

using namespace euphoria::core;

enum class Type
{
    simple,
    grid
};


int
main(int argc, char* argv[])
{
    std::string file;
    std::string format = ",\"";
    Type type = Type::simple;
    CsvTrim trim = CsvTrim::trim;

    {
        auto parser = argparse::Parser {"csvtool"};

        parser.add("-format", &format).set_help("The CSV format used");
        parser.set_const("-simple", &type, Type::simple);
        parser.set_const("-grid", &type, Type::grid);
        parser.set_const("-notrim", &trim, CsvTrim::dont_trim);
        parser.add("CSV-file", &file);
        if(const auto r = parser.parse(argc, argv))
        {
            return *r;
        }
    }

    
    Table<std::string> table;
    {
        const auto contents = io::file_to_string(file);
        if(!contents)
        {
            std::cerr << "Failed to load " << file << "\n";
            return -1;
        }
        CsvParserOptions options;
        options.delim = format[0];
        options.str = format[1];
        options.trim = trim;
        table = table_from_csv(*contents, options);
    }

    switch(type)
    {
    case Type::simple: print_table_simple(std::cout, table); break;
    case Type::grid: print_table_grid(std::cout, table); break;
    }

    return 0;
}
