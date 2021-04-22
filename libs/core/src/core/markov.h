#pragma once

#include "core/random.h"
#include "core/assert.h"

#include <vector>
#include <map>
#include <deque>
#include <memory>

namespace euphoria::core::markov
{
    template <typename T>
    struct some
    {
        std::deque<T> value;
        size_t max;

        explicit some(size_t m) : max(m) {}

        void
        add(T t)
        {
            if(value.size() == max)
            {
                value.pop_front();
            }
            ASSERTX(value.size() < max, max, value.size());
            value.push_back(t);
        }
    };

    template <typename T>
    struct probability
    {
        std::map<T, float> data;

        T
        get(core::Random* rnd) const
        {
            float value = rnd->NextFloat01();
            for(auto it: data)
            {
                if(value <= it.second)
                {
                    return it.first;
                }
                value -= it.second;
            }

            return data.rbegin()->first;
        }
    };

    template <typename T>
    struct probability_builder
    {
        int total = 0;
        std::map<T, int> data;

        void
        add(const T& t)
        {
            total += 1;

            auto r = data.find(t);
            if(r == data.end())
            {
                data[t] = 1;
            }
            else
            {
                data[t] = r->second + 1;
            }
        }

        probability<T>
        build() const
        {
            probability<T> r;
            for(auto it: data)
            {
                r.data[it.first] = it.second / static_cast<float>(total);
            }
            return r;
        }
    };

    template <typename T>
    struct chain
    {
        std::map<std::deque<T>, probability<std::shared_ptr<T>>> next;
        size_t order;

        std::vector<T>
        generate(core::Random* rnd) const
        {
            std::vector<T> r;
            some<T> memory {order};
            while(true)
            {
                auto found = next.find(memory.value);
                ASSERT(found != next.end());
                auto n = found->second.get(rnd);
                if(n == nullptr)
                {
                    return r;
                }

                r.emplace_back(*n);
                memory.add(*n);
            }
        }
    };

    template <typename T>
    struct chain_builder
    {
        std::map<std::deque<T>, probability_builder<std::shared_ptr<T>>> next;
        size_t order;

        chain_builder(int o) : order(o) {}

        void
        add(const std::vector<T>& ts)
        {
            ASSERT(!ts.empty());
            some<T> memory {order};

            for(const auto& t: ts)
            {
                next[memory.value].add(std::make_shared<T>(t));

                memory.add(t);
            }

            next[memory.value].add(nullptr);
        }

        chain<T>
        build() const
        {
            chain<T> r;
            r.order = order;
            for(auto n: next)
            {
                r.next[n.first] = n.second.build();
            }
            return r;
        }
    };

}
