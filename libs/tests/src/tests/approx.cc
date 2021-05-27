#include "tests/approx.h"

namespace
{
    // Performs equivalent check of std::fabs(lhs - rhs) <= margin
    // But without the subtraction to allow for INFINITY in comparison
    bool
    marginComparison(double lhs, double rhs, double margin)
    {
        return (lhs + margin >= rhs) && (rhs + margin >= lhs);
    }
} // namespace


namespace euphoria::tests
{
    template <>
    bool
    approximately_equal(
            double const& lhs,
            double const& rhs,
            const approx_data& data)
    {
        const auto m_value = lhs;
        const auto other = rhs;
        const auto m_epsilon = data.epsilon;
        const auto m_margin = data.margin;
        const auto m_scale = data.scale;
        // implementation:

        // First try with fixed margin, then compute margin based on epsilon, scale and Approx's value
        // Thanks to Richard Harris for his help refining the scaled margin value
        return marginComparison(m_value, other, m_margin)
               || marginComparison(
                       m_value,
                       other,
                       m_epsilon * (m_scale + std::fabs(m_value)));
    }

    template <>
    bool
    approximately_equal(
            float const& lhs,
            float const& rhs,
            const approx_data& data)
    {
        return approximately_equal(
                static_cast<double>(lhs), static_cast<double>(rhs), data);
    }
}
