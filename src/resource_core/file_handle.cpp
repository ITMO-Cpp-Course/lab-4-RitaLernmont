#include "file_handle.hpp"
#include "resource_common.hpp"
#include <fstream>
#include <sstream>

using namespace lab4::resource;

struct FileHandle::Impl
{
    std::string filepath;
    std::fstream stream;
    bool open;

    Impl(const std::string& path) : filepath(path), open(false)
    {
        stream.open(path, std::ios::in | std::ios::out |
                              std::ios::app); // или для std::ios(открыть для чтения, для записи, добавлятьв конец файла
                                              // при записи) которые кидают биты
        if (!stream.is_open())
        {
            stream.clear();                   // чистим ошибки
            stream.open(path, std::ios::out); // создаем новый файл через out
            if (!stream.is_open())
            {
                throw ResourceError("Failed to open file:" + path);
            }
            stream.close(); // закрываем
            stream.open(path,
                        std::ios::in | std::ios::out | std::ios::app); // файл существует точно, надо попробовать снова
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
            try // код может кинуть исключения
            {
                stream.close();
            }
            catch (...) // ловит исключения и ничего с ними не делает
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