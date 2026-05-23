#include "resource_manager.hpp"
#include "resource_common.hpp"
#include <fstream>
#include <sstream>

using namespace lab4::resource;

ResourceManager& ResourceManager::instance()
{
    static ResourceManager manager;
    return manager;
}

std::shared_ptr<FileHandle> ResourceManager::get_resource(const std::string& filepath)
{

    auto it = cache_.find(filepath);
    if (it != cache_.end())
    {
        auto ptr = it->second.lock();
        if (ptr)
        {
            return ptr;
        }
        cache_.erase(it);
    }

    auto new_ptr = std::make_shared<FileHandle>(filepath);
    cache_[filepath] = new_ptr;
    return new_ptr;
}

void ResourceManager::evict(const std::string& filepath)
{
    cache_.erase(filepath);
}

void ResourceManager::cleanup()
{
    for (auto it = cache_.begin(); it != cache_.end();)
    {
        if (it->second.expired())
        {
            it = cache_.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

size_t ResourceManager::cache_size() const
{
    return cache_.size();
}