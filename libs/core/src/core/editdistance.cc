#include "core/editdistance.h"



#include <algorithm>
#include <memory>

#include "assert/assert.h"
#include "core/cint.h"

namespace euphoria::core
{
    int
    calc_edit_distance(const std::string& source, const std::string& target)
    {
        const int source_length = c_sizet_to_int(source.length());
        const int target_length = c_sizet_to_int(target.length());
        if(source_length == 0)
        {
            return target_length;
        }
        if(target_length == 0)
        {
            return source_length;
        }

        using Tmatrix = std::vector<std::vector<int>>;

        Tmatrix matrix(source_length + 1);

        for(int source_index = 0; source_index <= source_length; source_index++)
        {
            matrix[source_index].resize(target_length + 1);
        }

        for(int source_index = 0; source_index <= source_length; source_index++)
        {
            matrix[source_index][0] = source_index;
        }

        for(int target_index = 0; target_index <= target_length; target_index++)
        {
            matrix[0][target_index] = target_index;
        }

        for(int source_index = 1; source_index <= source_length; source_index++)
        {
            const char source_char = source[source_index - 1];

            for(int target_index = 1; target_index <= target_length; target_index++)
            {
                const char target_char = target[target_index - 1];

                const int cost = (source_char == target_char) ? 0 : 1;

                const int above = matrix[source_index - 1][target_index];
                const int left = matrix[source_index][target_index - 1];
                const int diag = matrix[source_index - 1][target_index - 1];
                int cell = std::min(above + 1, std::min(left + 1, diag + cost));

                if(source_index > 2 && target_index > 2)
                {
                    int trans = matrix[source_index - 2][target_index - 2] + 1;
                    if(source[source_index - 2] != target_char)
                    {
                        trans++;
                    }
                    if(source_char != target[target_index - 2])
                    {
                        trans++;
                    }
                    if(cell > trans)
                    {
                        cell = trans;
                    }
                }

                matrix[source_index][target_index] = cell;
            }
        }

        return matrix[source_length][target_length];
    }

    int
    calc_edit_distance_fast(const std::string& the_row, const std::string& the_col)
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
