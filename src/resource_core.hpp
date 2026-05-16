#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>

namespace lab4::resource{
    
    class ResourceError : public std::runtime_error
    {
    public:
        explicit ResourceError(const std::string& message);
        explicit ResourceError(const char* message);
    };
    
    class FileHandle
    {
    public:
        explicit FileHandle(const std::string& filepath);
        ~FileHandle();

        FileHandle(const FileHandle&) = delete;
        FileHandle& operator = (FileHandle&& other) = delete;

        FileHandle(FileHandle&& other) noexcept;
        FileHandle& operator = (FileHandle&& other) noexcept;

        void write(const std::string& data);
        std::string read();
        void flush();
        bool is_open() const noexcept;
        const std::string& get_filepath() const noexcept;
        
        private:
            struct Impl;
            std::unique_ptr<Impl> pimpl_; 
    };

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
        
        mutable std::mutex mutex_;
        std::unordered_map<std::string, std::weak_ptr<FileHandle>> cache_;
       
}