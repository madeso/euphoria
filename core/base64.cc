#include "base64.h"

#include <sstream>

// source: https://en.wikipedia.org/wiki/Base64

namespace base64
{
  namespace
  {
    const char* const CODES =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
  }

  std::string
  Encode(std::shared_ptr<MemoryChunk> memory)
  {
    MemoryChunk& in = *memory;
    // StringBuilder out = new StringBuilder((in.length * 4) / 3);
    std::ostringstream out;
    int                b;
    for(fuint64 i = 0; i < in.GetSize(); i += 3)
    {
      b = (in[i] & 0xFC) >> 2;
      out << CODES[b];
      b = (in[i] & 0x03) << 4;
      if(i + 1 < in.GetSize())
      {
        b |= (in[i + 1] & 0xF0) >> 4;
        out << CODES[b];
        b = (in[i + 1] & 0x0F) << 2;
        if(i + 2 < in.GetSize())
        {
          b |= (in[i + 2] & 0xC0) >> 6;
          out << CODES[b];
          b = in[i + 2] & 0x3F;
          out << CODES[b];
        }
        else
        {
          out << CODES[b];
          out << '=';
        }
      }
      else
      {
        out << CODES[b];
        out << "==";
      }
    }

    return out.str();
  }

  std::shared_ptr<MemoryChunk>
  Decode(const std::string& input)
  {
    const std::string codes = CODES;
    if(input.length() % 4 == 0)
    {
      auto    asize = (input.length() * 3) / 4;
      auto    found = input.find('=') != std::string::npos;
      auto    bsize = found ? (input.length() - input.find('=')) : 0;
      fuint64 size  = asize - bsize;

      auto          ret     = MemoryChunk::Alloc(size);
      MemoryChunk&  decoded = *ret;
      const auto&   inChars = input;
      int           j       = 0;
      unsigned long b[4]    = {
          0,
      };
      for(fuint64 i = 0; i < input.size(); i += 4)
      {
        // This could be made faster (but more complicated) by precomputing
        // these index locations.
        b[0]         = codes.find(inChars[i]);
        b[1]         = codes.find(inChars[i + 1]);
        b[2]         = codes.find(inChars[i + 2]);
        b[3]         = codes.find(inChars[i + 3]);
        decoded[j++] = static_cast<char>((b[0] << 2) | (b[1] >> 4));
        if(b[2] < 64)
        {
          decoded[j++] = static_cast<char>((b[1] << 4) | (b[2] >> 2));
          if(b[3] < 64)
          {
            decoded[j++] = static_cast<char>((b[2] << 6) | b[3]);
          }
        }
      }
      return ret;
    }
    else
    {
      // throw new IllegalArgumentException("Invalid base64 input");
      return MemoryChunk::Null();
    }
  }
}
