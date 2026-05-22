#include "resource_core.hpp"
#include <fstream>
#include <sstream>

using namespace lab4::resource;

ResourceError::ResourceError(const std::string& message) : std::runtime_error(message) {}

ResourceError::ResourceError(const char* message) : std::runtime_error(message) {}

struct FileHandle::Impl
{
    std::string filepath;
    std::fstream stream;
    bool open;

    Impl(const std::string& path) : filepath(path), open(false)
    {
        stream.open(path, std::ios::in | std::ios::out | std::ios::app);
        if (!stream.is_open())
        {
            stream.clear();
            stream.open(path, std::ios::out);
            if (!stream.is_open())
            {
                throw ResourceError("Failed to open file:" + path);
            }
            stream.close();
            stream.open(path, std::ios::in | std::ios::out | std::ios::app);
            if (!stream.is_open())
            {
                throw ResourceError("Failed to reopen file:" + path);
            }
        }
        open = true;
    };

    ~Impl()
    {
        if (open && stream.is_open())
        {
            try
            {
                stream.close();
            }
            catch (...)
            {
            }
        }
    };
};

FileHandle::FileHandle(const std::string& filepath) : pimpl_(std::make_unique<Impl>(filepath)) {}

FileHandle::~FileHandle() = default;

FileHandle::FileHandle(FileHandle&& other) noexcept : pimpl_(std::move(other.pimpl_)) {}

FileHandle& FileHandle::operator=(FileHandle&& other) noexcept
{
    if (this != &other)
    {
        pimpl_ = std::move(other.pimpl_);
    }
    return *this;
}

void FileHandle::write(const std::string& data)
{
    if (!pimpl_->open)
    {
        throw ResourceError("FileHandle is not open");
    }

    pimpl_->stream.clear();
    pimpl_->stream.seekp(0, std::ios::beg);
    pimpl_->stream << data;
    if (!pimpl_->stream)
    {
        throw ResourceError("Failed to write to open:" + pimpl_->filepath);
    }
}

std::string FileHandle::read()
{
    if (!pimpl_ || !pimpl_->open)
    {
        throw ResourceError("FileHandle is not open");
    }

    pimpl_->stream.clear();
    pimpl_->stream.seekp(0, std::ios::beg);

    std::stringstream buffer;
    buffer << pimpl_->stream.rdbuf();

    if (!pimpl_->stream && !pimpl_->stream.eof())
    {
        throw ResourceError("Failed to read from file:" + pimpl_->filepath);
    }

    return buffer.str();
}

void FileHandle::flush()
{
    if (pimpl_ && pimpl_->open && pimpl_->stream.is_open())
    {
        pimpl_->stream.flush();
    }
}

bool FileHandle::is_open() const noexcept
{
    return pimpl_ && pimpl_->open;
}

const std::string& FileHandle::get_filepath() const noexcept
{
    static const std::string empty;
    return pimpl_ ? pimpl_->filepath : empty;
}

ResourceManager& ResourceManager::instance()
{
    static ResourceManager manager;
    return manager;
}

std::shared_ptr<FileHandle> ResourceManager::get_resource(const std::string& filepath)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = cache_.find(filepath);
    if (it != cache_.end())
    {
        auto ptr = it->second.lock();
        if (ptr)
        {
            return ptr;
        }
    }

    auto new_ptr = std::make_shared<FileHandle>(filepath);
    cache_[filepath] = new_ptr;
    return new_ptr;
}

void ResourceManager::evict(const std::string& filepath)
{
    std::lock_guard<std::mutex> lock(mutex_);
    cache_.erase(filepath);
}

void ResourceManager::cleanup()
{
    std::lock_guard<std::mutex> lock(mutex_);
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
    std::lock_guard<std::mutex> lock(mutex_);
    return cache_.size();
}