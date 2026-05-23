#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace lab4::resource
{

class ResourceManager
{
  public:
    static ResourceManager& instance();

    std::shared_ptr<FileHandle> get_resource(const std::string& filepath);
    void evict(const std::string& filepath);
    void cleanup();
    size_t cache_size() const;

  private:
    ResourceManager() = default;
    ~ResourceManager() = default;

    std::unordered_map<std::string, std::weak_ptr<FileHandle>> cache_;
};
} // namespace lab4::resource