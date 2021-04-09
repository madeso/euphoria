#ifndef CORE_MARKOV_H
#define CORE_MARKOV_H

#include "core/random.h"
#include "core/assert.h"

#include <vector>
#include <map>
#include <deque>
#include <memory>

namespace euphoria::core::markov
{
    template <typename T>
    struct Some
    {
        std::deque<T> value;
        size_t max;
        explicit Some(size_t m) : max(m) {}
        void
        Add(T t)
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
    struct Probability
    {
        std::map<T, float> data;

        T
        Get(core::Random* rnd) const
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
    struct ProbabilityBuilder
    {
        int total = 0;
        std::map<T, int> data;

        void
        Add(const T& t)
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

        Probability<T>
        Build() const
        {
            Probability<T> r;
            for(auto it: data)
            {
                r.data[it.first] = it.second / static_cast<float>(total);
            }
            return r;
        }
    };

    template <typename T>
    struct Chain
    {
        std::map<std::deque<T>, Probability<std::shared_ptr<T>>> next;
        size_t order;

        std::vector<T>
        Generate(core::Random* rnd) const
        {
            std::vector<T> r;
            Some<T> memory {order};
            while(true)
            {
                auto found = next.find(memory.value);
                ASSERT(found != next.end());
                auto n = found->second.Get(rnd);
                if(n == nullptr)
                {
                    return r;
                }

                r.emplace_back(*n);
                memory.Add(*n);
            }
        }
    };

    template <typename T>
    struct ChainBuilder
    {
        std::map<std::deque<T>, ProbabilityBuilder<std::shared_ptr<T>>> next;
        size_t order;

        ChainBuilder(int o) : order(o) {}

        void
        Add(const std::vector<T>& ts)
        {
            ASSERT(!ts.empty());
            Some<T> memory {order};

            for(const auto& t: ts)
            {
                next[memory.value].Add(std::make_shared<T>(t));

                memory.Add(t);
            }

            next[memory.value].Add(nullptr);
        }

        Chain<T>
        Build() const
        {
            Chain<T> r;
            r.order = order;
            for(auto n: next)
            {
                r.next[n.first] = n.second.Build();
            }
            return r;
        }
    };

} // namespace euphoria::core::markov

#endif  // CORE_MARKOV_H
