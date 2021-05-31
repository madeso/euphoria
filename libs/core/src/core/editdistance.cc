#include "core/editdistance.h"

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

#include "core/assert.h"

namespace euphoria::core
{
    unsigned long
    edit_distance(const std::string& source, const std::string& target)
    {
        const unsigned long n = source.length();
        const unsigned long m = target.length();
        if(n == 0)
        {
            return m;
        }
        if(m == 0)
        {
            return n;
        }

        using Tmatrix = std::vector<std::vector<unsigned long>>;

        Tmatrix matrix(n + 1);

        for(unsigned long i = 0; i <= n; i++)
        {
            matrix[i].resize(m + 1);
        }

        for(unsigned long i = 0; i <= n; i++)
        {
            matrix[i][0] = i;
        }

        for(unsigned long j = 0; j <= m; j++)
        {
            matrix[0][j] = j;
        }

        for(unsigned long i = 1; i <= n; i++)
        {
            const char s_i = source[i - 1];

            for(unsigned long j = 1; j <= m; j++)
            {
                const char t_j = target[j - 1];

                const int cost = (s_i == t_j) ? 0 : 1;

                const unsigned long above = matrix[i - 1][j];
                const unsigned long left = matrix[i][j - 1];
                const unsigned long diag = matrix[i - 1][j - 1];
                unsigned long cell = std::min(above + 1, std::min(left + 1, diag + cost));

                if(i > 2 && j > 2)
                {
                    unsigned long trans = matrix[i - 2][j - 2] + 1;
                    if(source[i - 2] != t_j)
                    {
                        trans++;
                    }
                    if(s_i != target[j - 2])
                    {
                        trans++;
                    }
                    if(cell > trans)
                    {
                        cell = trans;
                    }
                }

                matrix[i][j] = cell;
            }
        }

        return matrix[n][m];
    }

    unsigned long
    fast_edit_distance(const std::string& the_row, const std::string& the_col)
    {
        const unsigned long row_length = the_row.length();
        const unsigned long col_length = the_col.length();

        if(row_length == 0)
        {
            return col_length;
        }

        if(col_length == 0)
        {
            return row_length;
        }

        std::unique_ptr<unsigned long[]> v0 {new unsigned long[row_length + 1]};
        std::unique_ptr<unsigned long[]> v1 {new unsigned long[row_length + 1]};

        for(unsigned long row_index = 0; row_index <= row_length; row_index++)
        {
            v0[row_index] = row_index;
            v1[row_index] = 0;
        }

        for(unsigned long col_index = 1; col_index <= col_length; col_index++)
        {
            v1[0] = col_index;

            for(unsigned long row_index = 1; row_index <= row_length; row_index++)
            {
                const int cost
                        = the_row[row_index - 1] == the_col[col_index - 1] ? 0 : 1;

                const unsigned long m_min = v0[row_index] + 1;
                const unsigned long b = v1[row_index - 1] + 1;
                const unsigned long c = v0[row_index - 1] + cost;

                v1[row_index] = std::min(std::min(m_min, b), c);
            }

            v0.swap(v1);
        }

        return v0[row_length];
    }

} // namespace euphoria::core
