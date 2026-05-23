#pragma once

#include "file_handle.hpp"
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace lab4::resource
{

class ResourceManager
{
  public:
    ~ResourceManager() = default;
    static ResourceManager& instance();

    std::shared_ptr<FileHandle> get_resource(const std::string& filepath);
    void evict(const std::string& filepath);
    void cleanup();
    size_t cache_size() const;

  private:
    ResourceManager() = default;

    std::unordered_map<std::string, std::weak_ptr<FileHandle>> cache_;
    mutable std::mutex mutex_; // тесты показали что у меня много потоков, поэтому нужна защита
};
} // namespace lab4::resource