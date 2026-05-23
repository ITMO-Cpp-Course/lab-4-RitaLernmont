#include "resource_common.hpp"
#include <stdexcept>

using namespace lab4::resource;

ResourceError::ResourceError(const std::string& message) : std::runtime_error(message) {}

ResourceError::ResourceError(const char* message) : std::runtime_error(message) {}