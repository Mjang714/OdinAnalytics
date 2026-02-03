/**
 * @file call.cpp
 * @author Derek Huang
 * @brief C++ source for functions corresponding to `Excel12()` calls
 * @copyright MIT License
 */

#include "oa/accel/call.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

#include <cstdint>
#include <string_view>
#include <vector>

#include "oa/accel/addin.h"
#include "oa/accel/menu.h"
#include "oa/accel/oper12.h"

namespace oa {
namespace accel {

std::uintptr_t window()
{
  oper12 res;
  Excel12(xlGetHwnd, &*res, 0);
  return static_cast<std::uintptr_t>(res->val.w);
}

void alert(std::string_view str)
{
  oper12 res;
  oper12 message{str};
  // note: return value ignored since we know it is always true
  Excel12(xlcAlert, &*res, 1, &*message);
}

bool alert(std::string_view str, alert_type type)
{
  oper12 res;
  oper12 message{str};
  oper12 style{static_cast<int>(type)};
  Excel12(xlcAlert, &*res, 2, &*message, &*style);
  return !!res->val.xbool;
}

bool worksheet_menu(std::string_view name)
{
  oper12 res;
  oper12 menu_id{10};      // worksheet menu bar ID
  oper12 menu_name{name};
  oper12 menu_pos{0};      // or xltypeMissing
  // get menu on worksheet menu bar
  Excel12(
    xlfGetBar,
    &*res,
    3,
    &*menu_id,
    &*menu_name,
    &*menu_pos
  );
  // if error then not found
  return !res.error();
}

bool worksheet_menu(const menu& m)
{
  // menu rows (name + command name)
  std::vector<oper12> rows{m.name(), ""};
  // register menu commands
  for (const auto& item : m) {
    // item name
    rows.emplace_back(item.text());
    rows.emplace_back(
      // if command is provided (non-empty), decorate with addin::stem() + _
      item.func().empty() ?
        std::string{""} :
        std::string{addin::stem()} + "_" + std::string{item.func()}
    );
  }
  // result, worksheet menu bar ID, menu data table
  oper12 res;
  oper12 menu_id{10};
  oper12 menu_data{{rows.data(), rows.size() / 2u, 2u}};
  Excel12(xlfAddMenu, &*res, 2, &*menu_id, &*menu_data);
  // if error then failed
  return !res.error();
}

bool delete_worksheet_menu(const menu& m)
{
  oper12 res;
  oper12 menu_id{10};                // worksheet menu bar ID
  oper12 menu_name{m.clean_name()};  // menu name without '&'
  Excel12(xlfDeleteMenu, &*res, 2, &*menu_id, &*menu_name);
  // if error then failed
  return !res.error();
}

}  // namespace accel
}  // namespace oa
