#pragma once




#include <numeric>
#include <algorithm>
#include <memory>

#include "core/quicksort.h"
#include "core/insertionsort.h"

namespace euphoria::core
{
    enum class SortStyle
    {
        ascending,
        descending
    };

    template <typename T>
    struct Sortable
    {
        Sortable() = default;
        virtual ~Sortable() = default;

        Sortable(const Sortable<T>&) = delete;
        Sortable(Sortable<T>&&) = delete;
        void operator=(const Sortable<T>&) = delete;
        void operator=(Sortable<T>&&) = delete;

        virtual int sort(const T& lhs, const T& rhs) = 0;
    };

    template <typename T>
    using SortableList = std::vector<std::shared_ptr<Sortable<T>>>;

    template <typename T, typename TValue, typename TSortFunc>
    struct SortAction : public Sortable<T>
    {
        TValue T::*member;
        core::SortStyle sort_style;
        TSortFunc sort_func;

        SortAction(TValue T::*m, core::SortStyle s, TSortFunc f)
            : member(m), sort_style(s), sort_func(f)
        {}

        int
        sort(const T& lhs, const T& rhs) override
        {
            const int result = sort_func(lhs.*member, rhs.*member);
            return sort_style == SortStyle::ascending ? result : -result;
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

    // todo(Gustav): refactor away crtp and inheritence?

    /**
    * @param TSelf self for a CRTP reference
    */
    template <typename T, typename TSelf>
    struct SortBuilder
    {
        SortableList<T> sort_order;
        bool stable_sort = false;

        template <typename TSortFunc, typename TValue>
        TSelf&
        sort
        (
            TValue T::*member,
            TSortFunc sort_func,
            SortStyle sort_style = SortStyle::ascending
        )
        {
            auto o = std::make_shared<SortAction<T, TValue, TSortFunc>>
            (
                member, sort_style, sort_func
            );
            sort_order.emplace_back(o);
            return static_cast<TSelf&>(*this);
        }

        template <typename TValue>
        TSelf&
        sort(TValue T::*member, SortStyle sort_style = SortStyle::ascending)
        {
            return sort(member, &default_sort_func<TValue>, sort_style);
        }

        TSelf&
        use_stable_sort()
        {
            stable_sort = true;
            return *this;
        }
    };

    template <typename T, typename TSelf>
    std::vector<size_t>
    get_sorted_indices
    (
        const std::vector<T>& data,
        const SortBuilder<T, TSelf>& builder
    )
    {
        std::vector<size_t> r(data.size());
        std::iota(std::begin(r), std::end(r), 0);
        if(builder.sort_order.empty())
        {
            return r;
        }
        const auto sort_function = [&data, &builder](size_t lhs, size_t rhs) -> int
        {
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

