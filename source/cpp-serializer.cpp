#include <string>

#include "cpp-serializer/cpp-serializer.hpp"

#include <fmt/core.h>

exported_class::exported_class()
    : m_name {fmt::format("{}", "cpp-serializer")}
{
}

auto exported_class::name() const -> char const*
{
  return m_name.c_str();
}
