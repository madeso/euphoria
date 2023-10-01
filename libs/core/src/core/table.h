#pragma once


#include "assert/assert.h"
#include "base/numeric.h"
#include "base/rect.h"
#include "base/cint.h"
#include "base/vec2.h"

namespace eu::core
{
    // NewCol function??? probably less useful than insert functions
    // todo(Gustav): add insert before/after row/col
    // todo(Gustav): add remove row/col

    template <typename T>
    struct Table
    {
        using Idx = int;

        Table() = default;

        [[nodiscard]] static Table from_width_height(Idx width, Idx height, T d = T())
        {
            return Table(width, height, d);
        }

        [[nodiscard]] static Table from_height_width(Idx height, Idx width, T d = T())
        {
            return Table(width, height, d);
        }

        void add_row(T d = T())
        {
            ASSERT(width > 0);
            data.resize(data.size() + width, d);
            height += 1;
        }

        void clear(T d = T())
        {
            for(size_t index = 0; index < data.size(); index += 1)
            {
                data[index] = d;
            }
        }

        template <typename TFunc>
        void set_all(TFunc f)
        {
            for(Idx y = 0; y < height; y += 1)
            {
                for(Idx x = 0; x < width; x += 1)
                {
                    (*this)[{x, y}] = f(x, y);
                }
            }
        }

        /** Add a new row to the table.
         * Places the new row at the bottom left and resizes the table if necessary.
         * Fills the (new) empty space with default values.
         * @param row the new row
         * @param d the default value (if needed)
         */
        void add_row(const std::vector<T>& row, T d = T())
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
                width = c_sizet_to_int(row.size());
                height = 1;
            }
            else if(width == conv(row.size()))
            {
                // the number of columns math the new new, just insert it
                data.insert(data.end(), row.begin(), row.end());
                height += 1;
            }
            else if(width > conv(row.size()))
            {
                // new row has less columns than the table, solve by expanding the new row with empties
                data.insert(data.end(), row.begin(), row.end());
                for(auto col = conv(row.size()); col < width; col += 1)
                {
                    data.push_back(d);
                }
                height += 1;
            }
            else
            {
                // new row has more columns, solve by expanding table with empties
                ASSERTX(width < conv(row.size()), width, row.size());
                expand_width(c_sizet_to_int(row.size()), d);
                data.insert(data.end(), row.begin(), row.end());
                height += 1;
            }
        }

        /** Expand the table to the new width.
         * Aligns existing content to the left, fills the empty space with default values.
         * @param new_width the new width
         * @param d the default value
         */
        void expand_width(Idx new_width, T d = T())
        {
            // todo(Gustav): only insert the columns we need
            ASSERT(width != 0);
            ASSERTX(width < new_width, width, new_width);

            auto t = Table<T>::from_width_height(new_width, height, d);
            for(Idx x = 0; x < width; x += 1)
            {
                for(Idx y = 0; y < height; y += 1)
                {
                    t[{x, y}] = (*this)[{x, y}];
                }
            }
            *this = t;
        }


        [[nodiscard]] bool is_inside(Idx x, Idx y) const
        {
            return get_indices().contains_inclusive(x, y);
        }

        [[nodiscard]] size_t get_data_index(const vec2i& pos) const
        {
            ASSERTX
            (
                is_within_inclusive_as_int(0, pos.x, width - 1) && is_within_inclusive_as_int(0, pos.y, height - 1),
                pos.x, width,
                pos.y, height
            );
            return pos.y * width + pos.x;
        }


        [[nodiscard]] Recti get_indices() const
        {
            return Recti::from_width_height(width - 1, height - 1);
        }

        [[nodiscard]] Idx get_width() const
        {
            return width;
        }

        [[nodiscard]] Idx get_height() const
        {
            return height;
        }

        

        T operator[](const vec2i& pos) const
        {
            const auto index = get_data_index(pos);
            ASSERTX(index < data.size(), index, data.size());
            return data[index];
        }

        typename std::vector<T>::reference operator[](const vec2i& pos)
        {
            const auto index = get_data_index(pos);
            ASSERTX(index < data.size(), index, data.size());
            return data[index];
        }


        [[nodiscard]] static constexpr Idx
        conv(size_t t)
        {
            return c_sizet_to_int(t);
        }

    private:
        // table is stored in in place like [row, row, ...row]
        std::vector<T> data;
        Idx width = 0;
        Idx height = 0;


        Table(Idx c, Idx r, T d = T()) : data(c * r, d), width(c), height(r)
        {
            ASSERTX(width >= 0 && height >= 0, width, height);
        }
    };

    template <typename T>
    std::vector<T> calc_column_as_vector(const Table<T>& t, typename Table<T>::Idx x)
    {
        ASSERTX(x < t.get_width(), x, t.get_width());
        std::vector<T> r;
        r.reserve(t.get_height());
        for(typename Table<T>::Idx y = 0; y < t.get_height(); ++y)
        {
            r.emplace_back(t[{x, y}]);
        }
        return r;
    }

    template <typename T>
    std::vector<T> calc_row_as_vector(const Table<T>& t, typename Table<T>::Idx y)
    {
        ASSERTX(y < t.get_height(), y, t.get_height());

        std::vector<T> r;
        r.reserve(t.get_width());
        for(typename Table<T>::Idx x = 0; x < t.get_width(); ++x)
        {
            r.emplace_back(t[{x, y}]);
        }
        return r;
    }

}
