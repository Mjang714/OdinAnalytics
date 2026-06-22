/**
 * @file menu.cpp
 * @author Derek Huang
 * @brief C++ source for an XLL add-in menu
 * @copyright MIT License
 */

#include "oa/accel/menu.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "oa/accel/addin.h"

namespace oa {
namespace accel {

////////////////////////////////////////////////////////////////////////////////
// menu::item                                                                 //
////////////////////////////////////////////////////////////////////////////////

menu::item::item(std::string text, std::string func)
  : text_{std::move(text)}, func_{std::move(func)}
{}

std::string_view
menu::item::text() const noexcept
{
  return text_;
}

std::string_view
menu::item::func() const noexcept
{
  return func_;
}

////////////////////////////////////////////////////////////////////////////////
// menu                                                                       //
////////////////////////////////////////////////////////////////////////////////

menu::menu(std::string name) : name_{std::move(name)}
{
  // add the "About..." command to the add-in. since this command will raise a
  // dialog we follow the convention of having trailing ellipses
  // note: accel_about() implemented in addin.cpp to decouple dependencies
  command("&About...", "accel_about");
  // add a separator for visual convenience
  separator();
}

std::string_view
menu::name() const noexcept
{
  if (name_.empty())
    return addin::instance().name();
  else
    return name_;
}

std::string
menu::clean_name() const
{
  std::stringstream ss;
  for (auto c : name())
    if (c != '&')
      ss << c;
  return std::move(ss).str();  // elide copy
}

menu&
menu::command(std::string text, std::string func)
{
  items_.emplace_back(std::move(text), std::move(func));
  return *this;
}

menu&
menu::separator()
{
  items_.emplace_back(std::string(++separators_, '-'));
  return *this;
}

menu::iterator
menu::begin() const noexcept
{
  return items_.begin();
}

menu::iterator
menu::end() const noexcept
{
  return items_.end();
}

std::size_t
menu::size() const noexcept
{
  return items_.size();
}

std::size_t
menu::separators() const noexcept
{
  return separators_;
}

addin&
menu::operator()() const
{
  return addin::instance();
}

}  // namespace accel
}  // namespace oa
