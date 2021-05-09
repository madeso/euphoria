#pragma once


#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <memory>

#include "core/quicksort.h"
#include "core/insertionsort.h"

namespace euphoria::core
{
    enum class sort_style
    {
        ascending,
        descending
    };

    template <typename T>
    struct sortable
    {
        sortable() = default;
        virtual ~sortable() = default;

        sortable(const sortable<T>&) = delete;
        sortable(sortable<T>&&) = delete;
        void operator=(const sortable<T>&) = delete;
        void operator=(sortable<T>&&) = delete;

        virtual int sort(const T& lhs, const T& rhs) = 0;
    };

    template <typename T>
    using sortable_list = std::vector<std::shared_ptr<sortable<T>>>;

    template <typename T, typename Value, typename SortFunc>
    struct sort_action : public sortable<T>
    {
        Value T::*member;
        core::sort_style sort_style;
        SortFunc  sort_func;

        sort_action(Value T::*m, core::sort_style s, SortFunc f)
            : member(m), sort_style(s), sort_func(f)
        {}

        int
        sort(const T& lhs, const T& rhs) override
        {
            const int result = sort_func(lhs.*member, rhs.*member);
            return sort_style == sort_style::ascending ? result : -result;
        }
    };

    template <typename T>
    int
    default_sort_func(T lhs, T rhs)
    {
        if(lhs == rhs)
        {
            return 0;
        }
        return lhs < rhs ? 1 : -1;
    }

    // todo(Gustav): this seems weird, replace with single argument? TableGeneration needs Self?
    template <typename T, typename Self>
    struct sort_builder
    {
        sortable_list<T> sort_order;
        bool stable_sort = false;

        template <typename SortFunc, typename Value>
        Self&
        sort(Value T::*member,
             SortFunc  sort_func,
             sort_style sort_style = sort_style::ascending)
        {
            auto o = std::make_shared<sort_action<T, Value, SortFunc>>(
                    member, sort_style, sort_func);
            sort_order.emplace_back(o);
            return static_cast<Self&>(*this);
        }

        template <typename Value>
        Self&
        sort(Value T::*member, sort_style sort_style = sort_style::ascending)
        {
            return sort(member, &default_sort_func<Value>, sort_style);
        }

        Self&
        use_stable_sort()
        {
            stable_sort = true;
            return *this;
        }
    };

    template <typename T, typename Self>
    std::vector<size_t>
    get_sorted_indices
    (
        const std::vector<T>& data,
        const sort_builder<T, Self>& builder
    )
    {
        std::vector<size_t> r(data.size());
        std::iota(std::begin(r), std::end(r), 0);
        if(builder.sort_order.empty())
        {
            return r;
        }
        const auto sort_function
                = [&data, &builder](size_t lhs, size_t rhs) -> int {
            for(const auto& so: builder.sort_order)
            {
                const auto result = so->sort(data[lhs], data[rhs]);
                if(result != 0)
                {
                    return -result;
                }
            }
            return 0;
        };
        // is a stable sort is requested, use insertion sort, otherwise use the faster quick sort
        if(builder.stable_sort)
        {
            insertion_sort(&r, sort_function);
        }
        else
        {
            quicksort(&r, sort_function);
        }
        return r;
    }

}

