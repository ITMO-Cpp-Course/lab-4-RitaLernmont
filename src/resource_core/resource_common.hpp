#pragma once

#include <stdexcept>
#include <string>

namespace lab4::resource
{
class ResourceError : public std::runtime_error
{
  public:
    explicit ResourceError(const std::string& message); // для std::string
    explicit ResourceError(const char* message);        // для с-строки
};
} // namespace lab4::resource