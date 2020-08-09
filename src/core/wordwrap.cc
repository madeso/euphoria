#include "core/wordwrap.h"


namespace euphoria::core
{
    std::vector<std::string>
    WordWrap
    (
        const std::string& str,
        std::function<bool (const std::string&)> measure
    )
    {
        using I = std::string::size_type;
        if(measure(str)) { return {str}; }

        auto ret = std::vector<std::string>{};

        I start = 0;
        I last = 0;

        auto get_string = [&str, &start](I end)
        {
            if(end == std::string::npos)
            {
                return str.substr(start);
            }
            else
            {
                return str.substr(start, end - (start+1));
            }
        };

        while(true)
        {
            if(last == std::string::npos)
            {
                return ret;
            }
            const auto first = str.find(' ', last);
            const auto s = get_string(first);
            const auto fit = measure(s);

            if(fit)
            {
                // fit.. continue
                last = (first != std::string::npos) ? first+1 : first;
            }
            if(!fit || last == std::string::npos)
            {
                // new doesnt fit, use last
                const auto p = last == start ? first : last;
                const auto pp = last == start && p != std::string::npos ? p+1 : p;
                ret.emplace_back(get_string(pp));
                if(p == std::string::npos)
                {
                    return ret;
                }
                if(last == start)
                {
                    start = p+1;
                    last = p;
                }
                else start = p;
            }
        }

        return ret;
    }
}

