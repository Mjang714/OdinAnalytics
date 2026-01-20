/**
 * @file udf.cpp
 * @author Derek Huang
 * @brief C++ source for XLL UDF export and registration helpers
 * @copyright MIT License
 */

#include "oa/accel/udf.h"

#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace oa {
namespace accel {

////////////////////////////////////////////////////////////////////////////////
// udf_arg                                                                    //
////////////////////////////////////////////////////////////////////////////////

udf_arg::udf_arg(std::string name, std::string help) noexcept
  : name_{std::move(name)}, help_{std::move(help)}
{}

std::string_view
udf_arg::name() const noexcept
{
  return name_;
}

std::string_view
udf_arg::help() const noexcept
{
  return name_;
}

////////////////////////////////////////////////////////////////////////////////
// udf                                                                        //
////////////////////////////////////////////////////////////////////////////////

std::string_view
udf::export_name() const noexcept
{
  return export_name_;
}

std::string_view
udf::type_template() const noexcept
{
  return type_template_;
}

std::string
udf::type_text() const
{
  // start with type_template()
  std::string text{type_template()};
  // add volatile + thread-safe qualifiers as appropriate
  if (thread_safe())
    text.push_back('$');
  if (volatile_())
    text.push_back('!');
  // done
  return text;
}

udf&
udf::name(std::string text) noexcept
{
  name_ = std::move(text);
  return *this;
}

std::string_view
udf::name() const noexcept
{
  return name_;
}

udf&
udf::type(int v)
{
  switch (v) {
  case 0:
  case 1:
  case 2:
    type_ = v;
    break;
  default:
    throw std::runtime_error{"only values 0, 1, 2 are allowed"};
  }
  return *this;
}

int
udf::type() const noexcept
{
  return type_;
}

udf&
udf::category(std::string text) noexcept
{
  category_ = std::move(text);
  return *this;
}

std::string_view
udf::category() const noexcept
{
  return category_;
}

udf&
udf::arg(std::string name, std::string help)
{
  args_.emplace_back(name, help);
  return *this;
}

udf&
udf::help_topic(std::string text) noexcept
{
  help_topic_ = std::move(text);
  return *this;
}

std::string_view
udf::help_topic() const noexcept
{
  return help_topic_;
}

udf&
udf::help(std::string text) noexcept
{
  help_ = std::move(text);
  return *this;
}

std::string_view
udf::help() const noexcept
{
  return help_;
}

udf&
udf::volatile_(bool v) noexcept
{
  is_volatile_ = v;
  return *this;
}

bool
udf::volatile_() const noexcept
{
  return is_volatile_;
}

udf&
udf::thread_safe(bool v) noexcept
{
  thread_safe_ = v;
  return *this;
}

bool
udf::thread_safe() const noexcept
{
  return thread_safe_;
}

////////////////////////////////////////////////////////////////////////////////
// udf_registry                                                               //
////////////////////////////////////////////////////////////////////////////////

udf_registry::iterator
udf_registry::begin() const
{
  return udfs_.begin();
}

udf_registry::iterator
udf_registry::end() const
{
  return udfs_.end();
}

udf&
udf_registry::back()
{
  return udfs_.back();
}

}  // namespace accel
}  // namespace oa
