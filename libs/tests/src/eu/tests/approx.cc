#include "eu/tests/approx.h"

namespace
{
    // Performs equivalent check of std::fabs(lhs - rhs) <= margin
    // But without the subtraction to allow for INFINITY in comparison
    bool
    margin_comparison(float lhs, float rhs, float margin)
    {
        return (lhs + margin >= rhs) && (rhs + margin >= lhs);
    }

    bool is_bad(float num)
    {
        if (std::isnan(num)) return true;
        if (std::isinf(num)) return true;
        return false;
    }
}


namespace eu::tests
{
    template <>
    bool
    is_approximately_equal
    (
        float const& lhs,
        float const& rhs,
        const ApproxData& data
    )
    {
        if (is_bad(lhs)) return false;
        if (is_bad(rhs)) return false;

        const auto m_value = lhs;
        const auto other = rhs;
        const auto m_epsilon = data.epsilon;
        const auto m_margin = data.margin;
        const auto m_scale = data.scale;

        // implementation:

        // First try with fixed margin, then compute margin based on epsilon, scale and Approx's value
        // Thanks to Richard Harris for his help refining the scaled margin value
        return
            margin_comparison(m_value, other, m_margin)
            ||
            margin_comparison
            (
                m_value,
                other,
                m_epsilon * (m_scale + std::fabs(m_value))
            )
            ;
    }
}

namespace eu::tests::custom
{
    
}
