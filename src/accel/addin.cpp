/**
 * @file addin.cpp
 * @author Derek Huang
 * @brief C++ source for an XLL add-in framework class
 * @copyright MIT License
 */

#include "oa/accel/addin.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "oa/accel/oper12.h"
#include "oa/string.h"

namespace oa {
namespace accel {

namespace {

/**
 * Registry for all created `addin` instances.
 *
 * Since we cannot enforce at compile time that a static `addin` is constructed
 * only *once* we instead just track every allocated `addin`.
 *
 * Correctly implemented Accel XLLs will have `addin_registry().size()` be 1.
 */
auto& addin_registry()
{
  static std::vector<addin*> refs;
  return refs;
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// functions                                                                  //
////////////////////////////////////////////////////////////////////////////////

std::uintptr_t excel_window()
{
  static auto handle = []
  {
    oper12 res;
    Excel12(xlGetHwnd, res.value(), 0);
    return static_cast<std::uintptr_t>(res.value()->val.w);
  }();
  return handle;
}

////////////////////////////////////////////////////////////////////////////////
// addin                                                                      //
////////////////////////////////////////////////////////////////////////////////

addin::addin()
{
  addin_registry().push_back(this);
}

addin::~addin()
{
  // note: if there is an exception just crash
  addin_registry().pop_back();
}

addin&
addin::instance()
{
  return *addin_registry().front();
}

std::string_view
addin::path()
{
  static auto path_ = []
  {
    // wide string Excel path
    auto wstr = wpath();
    // allocate new string, narrow, and return
    std::string str(wstr.size(), '\0');
    for (auto i = 0u; i < wstr.size(); i++)
      str[i] = std::wcout.narrow(wstr[i], '?');  // note:
    return str;
  }();
  // return view
  return path_;
}

std::wstring_view
addin::wpath()
{
  static auto path = []
  {
    oper12 op;
    Excel12(xlGetName, op.value(), 0);
    return op;
  }();
  // string size in first code point
  return &path.value()->val.str[1];
}

std::string
addin::info() const
{
  std::stringstream ss;
  ss << "Accel add-in at 0x" << hex << reinterpret_cast<std::uintptr_t>(this);
  return ss.str();
}

void
addin::on_auto_free(const xloper12* /*op*/)
{
  /* no-op */;
}

////////////////////////////////////////////////////////////////////////////////
// XLL interface functions                                                    //
////////////////////////////////////////////////////////////////////////////////

extern "C" {

OA_XLL_EXPORT(xloper12*) xlAddInManagerInfo12(xloper12* op) OA_ACCEL_SAFE()
{
  // TODO: add try-catch for exceptions
  // coerced input from Excel + xlCoerce bitmask
  oper12 in;
  oper12 mask{xltypeInt};
  // coerce input using mask
  Excel12(xlCoerce, in.value(), 2, op, mask.value());
  // if 1, return XLL name, otherwise return #VALUE!
  auto res = [&in]
  {
    if (in.value()->val.w == 1)
      return oper12{addin::instance().info()};
    else
      return oper12{xlerr::value};
  }();
  // release raw value back to excel
  OA_ACCEL_SAFE_RETURN(res.release());
}

/**
 * Excel callback to deallocate any `oper12` released back to Excel.
 */
OA_XLL_EXPORT(void) xlAutoFree12(xloper12* op) noexcept
{
  auto& xll = addin::instance();
  // pre-free event
  xll.on_auto_free(op);
  // note: xlbitDLLFree should already be set
  xloper12_free(op);
}

/**
 * Excel callback required by every XLL for add-in activation.
 *
 * @todo Obviously missing function registration.
 */
OA_XLL_EXPORT(int) xlAutoOpen() noexcept
{
  // if add-in registry is empty, no addin instance was constructed. this is an
  // error, so pop up a message box to inform the user of such
  if (addin_registry().empty()) {
    // note: don't need to check return
    MessageBoxA(
      reinterpret_cast<HWND>(excel_window()),
      "No Accel addin instance was registered.\n"
      "\n"
      "Excel will load the XLL but no functions will be available.",
      "xlAutoOpen() error",
      MB_ICONWARNING
    );
    return 1;
  }
  // if more than one, also an error
  if (addin_registry().size() > 1) {
    // note: don't need to check return
    MessageBoxA(
      reinterpret_cast<HWND>(excel_window()),
      // note: due to temporary lifetime rules string lives until end of the
      // enclosing full-expression (to the semicolon)
      []
      {
        return (
          std::to_string(addin_registry().size()) +
          " Accel addin instances were registered.\n"
          "\n"
          "Only one Accel addin instance can be registered per XLL."
        );
      }()
      .c_str(),
      "xlAutoOpen() error",
      MB_ICONWARNING
    );
    return 1;
  }
  // TODO: fill in
  return 1;
}

}  // extern "C"

}  // namespace accel
}  // namespace oa
