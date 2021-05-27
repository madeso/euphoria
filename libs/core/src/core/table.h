#ifndef CORE_TABLE_H
#define CORE_TABLE_H

#include <vector>

#include "core/assert.h"
#include "core/numeric.h"
#include "core/rect.h"
#include "core/cint.h"

namespace euphoria::core
{
    template <typename T>
    struct table
    {
        using I = int;

        [[nodiscard]] static constexpr I
        Ci(size_t t)
        {
            return Csizet_to_int(t);
        }

        table() = default;

        [[nodiscard]] static table
        from_width_height(I width, I height, T d = T())
        {
            return table(width, height, d);
        }

        [[nodiscard]] static table
        from_height_width(I height, I width, T d = T())
        {
            return table(width, height, d);
        }

        void
        new_row(T d = T())
        {
            ASSERT(width > 0);
            data.resize(data.size() + width, d);
            height += 1;
        }

        void
        clear(T d = T())
        {
            for(size_t i = 0; i < data.size(); i += 1)
            {
                data[i] = d;
            }
        }

        template <typename Func>
        void
        set_all(Func f)
        {
            for(I y = 0; y < height; y += 1)
            {
                for(I x = 0; x < width; x += 1)
                {
                    (*this)(x, y) = f(x, y);
                }
            }
        }

        [[nodiscard]] recti
        get_indices() const
        {
            return recti::from_width_height(width - 1, height - 1);
        }

        [[nodiscard]] bool
        is_inside(I x, I y) const
        {
            return get_indices().contains_inclusive(x, y);
        }

        /** Add a new row to the table.
         * Places the new row at the bottom left and resizes the table if necessary.
         * Fills the (new) empty space with default values.
         * @param row the new row
         * @param d the default value (if needed)
         */
        void
        new_row(const std::vector<T>& row, T d = T())
        {
            // do nothing on empty rows
            if(row.empty())
            {
                return;
            }

            if(width == 0)
            {
                // this is a new table, the row is the new table
                ASSERTX(height == 0, height);
                data = row;
                width = row.size();
                height = 1;
            }
            else if(width == Ci(row.size()))
            {
                // the number of columns math the new new, just insert it
                data.insert(data.end(), row.begin(), row.end());
                height += 1;
            }
            else if(width > Ci(row.size()))
            {
                // new row has less columns than the table, solve by expanding the new row with empties
                data.insert(data.end(), row.begin(), row.end());
                for(auto i = Ci(row.size()); i < width; i += 1)
                {
                    data.push_back(d);
                }
                height += 1;
            }
            else
            {
                // new row has more columns, solve by expanding table with empties
                ASSERTX(width < Ci(row.size()), width, row.size());
                expand_width(row.size(), d);
                data.insert(data.end(), row.begin(), row.end());
                height += 1;
            }
        }

        /** Expand the table to the new width.
         * Aligns existing content to the left, fills the empty space with default values.
         * @param new_width the new width
         * @param d the default value
         */
        void
        expand_width(I new_width, T d = T())
        {
            // todo(Gustav): only insert the columns we need
            ASSERT(width != 0);
            ASSERTX(width < new_width, width, new_width);

            auto t = table<T>::from_width_height(new_width, height, d);
            for(I x = 0; x < width; x += 1)
            {
                for(I y = 0; y < height; y += 1)
                {
                    t(x, y) = (*this)(x, y);
                }
            }
            *this = t;
        }

        // NewCol function??? probably less useful than insert functions

        // todo(Gustav): insert before/after row/col
        // todo(Gustav): remove row/col


        T
        operator()(I x, I y) const
        {
            const auto index = data_index(x, y);
            ASSERTX(index < data.size(), index, data.size());
            return data[index];
        }

        typename std::vector<T>::reference
        operator()(I x, I y)
        {
            const auto index = data_index(x, y);
            ASSERTX(index < data.size(), index, data.size());
            return data[index];
        }

        // todo(Gustav): figure out a better name
        [[nodiscard]] I
        get_width() const
        {
            return width;
        }

        [[nodiscard]] I
        get_height() const
        {
            return height;
        }

        [[nodiscard]] size_t
        data_index(I x, I y) const
        {
            ASSERTX
            (
                is_within_inclusive_as_int(0, x, width - 1) && is_within_inclusive_as_int(0, y, height - 1),
                x, width,
                y, height
            );
            return y * width + x;
        }

    private:
        table(I c, I r, T d = T()) : data(c * r, d), width(c), height(r)
        {
            ASSERTX(width >= 0 && height >= 0, width, height);
        }

        // table is stored in in place like [row, row, ...row]
        std::vector<T> data;
        I width = 0;
        I height = 0;
    };

    template <typename T>
    std::vector<T>
    calc_column_as_vector(const table<T>& t, typename table<T>::I x)
    {
        ASSERTX(x < t.get_width(), x, t.get_width());
        std::vector<T> r;
        r.reserve(t.get_height());
        for(typename table<T>::I y = 0; y < t.get_height(); ++y)
        {
            r.emplace_back(t(x, y));
        }
        return r;
    }

    template <typename T>
    std::vector<T>
    calc_row_as_vector(const table<T>& t, typename table<T>::I y)
    {
        ASSERTX(y < t.get_height(), y, t.get_height());

        std::vector<T> r;
        r.reserve(t.get_width());
        for(typename table<T>::I x = 0; x < t.get_width(); ++x)
        {
            r.emplace_back(t(x, y));
        }
        return r;
    }

} // namespace euphoria::core

#endif // CORE_STRING_TABLE_H
