#pragma once

#include <map>
#include <memory>


namespace eu::core
{
    // todo(Gustav): support hotloading
    // todo(Gustav): rename to something better?
    // https://stackoverflow.com/questions/288400/guidelines-for-differences-between-cache-pool-cache-pool

    template <typename TKey, typename TData, typename TLoader>
    struct Cache
    {
        std::map<TKey, std::weak_ptr<TData>> store;
        
        std::shared_ptr<TData> get(const TKey& key)
        {
            auto found = store.find(key);
            if(found != store.end())
            {
                auto cached = found->second.lock();
                if(cached)
                {
                    return cached;
                }
                else
                {
                    store.erase(found);
                }
            }

            std::shared_ptr<TData> data = static_cast<TLoader *>(this)->create(key);
            store.insert(std::pair<TKey, std::weak_ptr<TData>>(key, data));
            return data;
        }
    };
}
