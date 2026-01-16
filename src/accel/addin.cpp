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

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "oa/accel/call.h"
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
  static auto ptr = []
  {
    // if registry is empty use default instance
    if (addin_registry().empty()) {
      static addin xll;
      return &xll;
    }
    // otherwise use first add-in registered
    else
      return addin_registry().front();
  }();
  return *ptr;
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
  // get full string path
  static auto path = []() -> std::wstring
  {
    // get path from Excel
    oper12 op;
    Excel12(xlGetName, op.value(), 0);
    // copy to wstring because calling Excel12() during static object
    // destruction is likely undefined behavior
    // note: Excel string size is in first code point + not null-terminated
    auto v = op.value();
    return {&v->val.str[1], static_cast<unsigned>(v->val.str[0])};
  }();
  // return view from conversion
  return path;
}

std::string
addin::long_name() const
{
  return std::filesystem::path{path()}.filename().string() + " dev";
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

/**
 * Excel callback run the first time the add-in manager is used in Excel.
 *
 * This provides the add-in long name to display in the add-in manager dialog.
 */
OA_XLL_EXPORT(xloper12*) xlAddInManagerInfo12(xloper12* op) OA_ACCEL_SAFE()
{
  // coerced input from Excel + xlCoerce bitmask
  oper12 in;
  oper12 mask{xltypeInt};
  // coerce input using mask
  Excel12(xlCoerce, in.value(), 2, op, mask.value());
  // if 1, return XLL long name, otherwise return #VALUE!
  auto res = [&in]
  {
    if (in.value()->val.w == 1)
      return oper12{addin::instance().long_name()};
    else
      return oper12{xlerr::value};
  }();
  // release raw value back to excel
  OA_ACCEL_SAFE_RETURN(res.release());
}

/**
 * Excel callback run whenever the add-in is activated via the add-in manager.
 *
 * This is not run when Excel starts up and initializes the add-in itself.
 */
OA_XLL_EXPORT(int) xlAutoAdd() noexcept
{
  alert(addin::instance().long_name() + " activated!");
  return 1;
}

/**
 * Excel callback run whenever the add-in is deactivated via add-in manager.
 *
 * This is not run when Excel is shutting down and deactivating add-ins.
 */
OA_XLL_EXPORT(int) xlAutoRemove() noexcept
{
  alert(addin::instance().long_name() + " deactivated!");
  return 1;
}

/**
 * Excel callback to deallocate any `oper12` released back to Excel.
 *
 * This calls the customization hook for user add-ins before `xloper12_free()`.
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
  // if more than one instance registered warn the user
  if (addin_registry().size() > 1)
    alert(
      (
        "Warning:\n"
        "\n"
        "(from " + addin::instance().long_name() + ")\n"
        "\n"
        "More than one Accel addin instance registered.\n"
        "Only one instance may be registered per XLL."
      )
      .c_str()
    );
  // TODO: fill in with function/macro registration + UI customizations
  return 1;
}

// TODO: add xlAutoClose() to unregister and undo customizations

}  // extern "C"

}  // namespace accel
}  // namespace oa
