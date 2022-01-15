#include "core/editdistance.h"

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

#include "core/assert.h"
#include "core/cint.h"

namespace euphoria::core
{
    int
    edit_distance(const std::string& source, const std::string& target)
    {
        const int n = c_sizet_to_int(source.length());
        const int m = c_sizet_to_int(target.length());
        if(n == 0)
        {
            return m;
        }
        if(m == 0)
        {
            return n;
        }

        using Tmatrix = std::vector<std::vector<int>>;

        Tmatrix matrix(n + 1);

        for(int i = 0; i <= n; i++)
        {
            matrix[i].resize(m + 1);
        }

        for(int i = 0; i <= n; i++)
        {
            matrix[i][0] = i;
        }

        for(int j = 0; j <= m; j++)
        {
            matrix[0][j] = j;
        }

        for(int i = 1; i <= n; i++)
        {
            const char s_i = source[i - 1];

            for(int j = 1; j <= m; j++)
            {
                const char t_j = target[j - 1];

                const int cost = (s_i == t_j) ? 0 : 1;

                const int above = matrix[i - 1][j];
                const int left = matrix[i][j - 1];
                const int diag = matrix[i - 1][j - 1];
                int cell = std::min(above + 1, std::min(left + 1, diag + cost));

                if(i > 2 && j > 2)
                {
                    int trans = matrix[i - 2][j - 2] + 1;
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

    int
    fast_edit_distance(const std::string& the_row, const std::string& the_col)
    {
        const int row_length = c_sizet_to_int(the_row.length());
        const int col_length = c_sizet_to_int(the_col.length());

        if(row_length == 0)
        {
            return col_length;
        }

        if(col_length == 0)
        {
            return row_length;
        }

        std::unique_ptr<int[]> v0 {new int[row_length + 1]};
        std::unique_ptr<int[]> v1 {new int[row_length + 1]};

        for(int row_index = 0; row_index <= row_length; row_index++)
        {
            v0[row_index] = row_index;
            v1[row_index] = 0;
        }

        for(int col_index = 1; col_index <= col_length; col_index++)
        {
            v1[0] = col_index;

            for(int row_index = 1; row_index <= row_length; row_index++)
            {
                const int cost
                        = the_row[row_index - 1] == the_col[col_index - 1] ? 0 : 1;

                const int m_min = v0[row_index] + 1;
                const int b = v1[row_index - 1] + 1;
                const int c = v0[row_index - 1] + cost;

                v1[row_index] = std::min(std::min(m_min, b), c);
            }

            v0.swap(v1);
        }

        return v0[row_length];
    }

}
