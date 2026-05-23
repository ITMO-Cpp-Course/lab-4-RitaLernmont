#pragma once

#include <memory>
#include <string>

class FileHandle
{
  public:
    explicit FileHandle(const std::string& filepath);
    ~FileHandle();

    FileHandle(const FileHandle&) = delete; //запрещает копировать
    FileHandle& operator=(const FileHandle&) = delete; //запрещает копировать

    FileHandle(FileHandle&& other) noexcept; //дает перемещать
    FileHandle& operator=(FileHandle&& other) noexcept; //дает перемещать

    void write(const std::string& data);
    std::string read();
    void flush();
    bool is_open() const noexcept;
    const std::string& get_filepath() const noexcept;

  private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};