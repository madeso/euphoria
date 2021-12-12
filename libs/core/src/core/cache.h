#pragma once

#include <map>
#include <memory>


namespace euphoria::core
{
    // todo(Gustav): support hotloading
    // todo(Gustav): rename to something better?
    // https://stackoverflow.com/questions/288400/guidelines-for-differences-between-cache-pool-cache-pool

    template <typename Key, typename Data, typename Loader>
    struct Cache
    {
        std::shared_ptr<Data>
        get(const Key& key)
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

            std::shared_ptr<Data> data = static_cast<Loader *>(this)->create(key);
            store.insert(std::pair<Key, std::weak_ptr<Data>>(key, data));
            return data;
        }

        std::map<Key, std::weak_ptr<Data>> store;
    };
}
