#include "core/guid.h"

#include "core/textfileparser.h"


#ifdef _WIN32
    #include <objbase.h>
#else
    #include <uuid/uuid.h>
#endif


namespace eu::core
{
#ifndef _WIN32
    std::optional<Guid> Guid::create()
    {
        Guid guid {};
        uuid_generate(guid.data.data());
        return guid;
    }
#else
    std::optional<Guid> Guid::create()
    {
        GUID g;

        if (S_OK != CoCreateGuid(&g))
        {
            return std::nullopt;
        }

        std::array<uint8_t, 16> data
        {
            static_cast<std::uint8_t>((g.Data1 >> 24) & 0xff),
            static_cast<std::uint8_t>((g.Data1 >> 16) & 0xff),
            static_cast<std::uint8_t>((g.Data1 >>  8) & 0xff),
            static_cast<std::uint8_t>((g.Data1      ) & 0xff),
            static_cast<std::uint8_t>((g.Data2 >> 8 ) & 0xff),
            static_cast<std::uint8_t>((g.Data2      ) & 0xff),
            static_cast<std::uint8_t>((g.Data3 >> 8 ) & 0xff),
            static_cast<std::uint8_t>((g.Data3      ) & 0xff),
            static_cast<std::uint8_t>( g.Data4[0]           ),
            static_cast<std::uint8_t>( g.Data4[1]           ),
            static_cast<std::uint8_t>( g.Data4[2]           ),
            static_cast<std::uint8_t>( g.Data4[3]           ),
            static_cast<std::uint8_t>( g.Data4[4]           ),
            static_cast<std::uint8_t>( g.Data4[5]           ),
            static_cast<std::uint8_t>( g.Data4[6]           ),
            static_cast<std::uint8_t>( g.Data4[7]           )
        };
        
        return Guid{data};
    }
#endif

    std::optional<Guid> Guid::parse(const std::string& str)
    {
        const auto skip = [](TextfileParser* parser, char c)
        {
            while(parser->peek_char() == c)
            {
                parser->read_char();
            }
        };
        const auto read_hex = [](TextfileParser* parser) -> std::optional<uint8_t>
        {
            const auto as_hex = [](char c) -> std::optional<uint8_t>
            {
                if ('0' <= c && c <= '9') { return static_cast<uint8_t>(c - '0'); }
                if ('a' <= c && c <= 'f') { return static_cast<uint8_t>(10 + (c - 'a')); }
                if ('A' <= c && c <= 'F') { return static_cast<uint8_t>(10 + (c - 'A')); }
                return std::nullopt;
            };
            const auto first = as_hex(parser->read_char());
            if(first.has_value() == false) { return std::nullopt; }

            const auto second = as_hex(parser->peek_char());
            if(second.has_value())
            {
                parser->read_char();
                return static_cast<uint8_t>((*first << 4) | *second);
            }
            else
            {
                return *first;
            }
        };
        auto parser = TextfileParser::from_string(str);
        parser.skip_spaces(false);
        skip(&parser, '{');

        Guid ret {};
        for(int index=0; index<16; index+=1)
        {
            if(parser.has_more() == false) { return std::nullopt; }
            
            auto hex = read_hex(&parser);
            if(hex.has_value() == false) { return std::nullopt; }

            ret.data[index] = *hex;
            parser.skip_spaces(false);
            skip(&parser, '-');
            parser.skip_spaces(false);
        }

        return ret;
    }

    std::string Guid::to_string() const
    {
        return fmt::format
        (
            "{:02x}{:02x}{:02x}{:02x}-"
            "{:02x}{:02x}-"
            "{:02x}{:02x}-"
            "{:02x}{:02x}-"
            "{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
            data[0], data[1], data[2], data[3],
            data[4], data[5],
            data[6], data[7],
            data[8], data[9],
            data[10], data[11], data[12], data[13], data[14], data[15]
        );
    }
}
