#include "base/guid.h"


#ifdef _WIN32
    #include <objbase.h>
#else
    #include <uuid/uuid.h>
#endif


namespace eu
{
#ifndef _WIN32
    std::optional<Guid> Guid::generate()
    {
        Guid guid {};
        uuid_generate(guid.data.data());
        return guid;
    }
#else
    std::optional<Guid> Guid::generate()
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

    bool operator==(const Guid& lhs, const Guid& rhs)
    {
        for(int i=0; i<16; i+=1)
        {
            if(lhs.data[i] != rhs.data[i]) return false;
        }
        return true;
    }

    bool operator!=(const Guid& lhs, const Guid& rhs)
    {
        return !(lhs == rhs);
    }

    struct Parser
    {
        std::string_view str;

        explicit Parser(const std::string_view s) : str(s) {}

        bool has_more() const
        {
            return str.empty() == false;
        }

        char read()
        {
            if(has_more() == false) return '\0';
            const char r = str[0];
            str = str.substr(1);
            return r;
        }

        char peek() const
        {
            if(has_more() == false) return '\0';
            return str[0];
        }

        void skip(char c)
        {
            while(peek() == c)
            {
                read();
            }
        }

        void skip_spaces()
        {
            skip(' ');
        }

        static std::optional<uint8_t> as_hex(char c)
        {
            if ('0' <= c && c <= '9') { return static_cast<uint8_t>(c - '0'); }
            if ('a' <= c && c <= 'f') { return static_cast<uint8_t>(10 + (c - 'a')); }
            if ('A' <= c && c <= 'F') { return static_cast<uint8_t>(10 + (c - 'A')); }
            return std::nullopt;
        };

        const std::optional<uint8_t> read_hex()
        {
            const auto first = as_hex(read());
            if(first.has_value() == false) { return std::nullopt; }

            const auto second = as_hex(peek());
            if(second.has_value())
            {
                read();
                return static_cast<uint8_t>((*first << 4) | *second);
            }
            else
            {
                return *first;
            }
        }
    };

    std::optional<Guid> guid_from(const std::string& str)
    {
        auto parser = Parser{str};
        parser.skip_spaces();
        parser.skip('{');
        parser.skip_spaces();

        Guid ret {};
        for(int index=0; index<16; index+=1)
        {
            if(parser.has_more() == false) { return std::nullopt; }
            
            auto hex = parser.read_hex();
            if(hex.has_value() == false) { return std::nullopt; }

            ret.data[index] = *hex;
            parser.skip_spaces();
            parser.skip('-');
            parser.skip_spaces();
        }

        return ret;
    }

    std::string string_from(const Guid& x)
    {
        return fmt::format
        (
            "{:02x}{:02x}{:02x}{:02x}-"
            "{:02x}{:02x}-"
            "{:02x}{:02x}-"
            "{:02x}{:02x}-"
            "{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
            x.data[0], x.data[1], x.data[2], x.data[3],
            x.data[4], x.data[5],
            x.data[6], x.data[7],
            x.data[8], x.data[9],
            x.data[10], x.data[11], x.data[12], x.data[13], x.data[14], x.data[15]
        );
    }
}
