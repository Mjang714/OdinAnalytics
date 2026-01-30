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

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "oa/accel/call.h"
#include "oa/accel/menu.h"
#include "oa/accel/oper12.h"
#include "oa/accel/udf.h"
#include "oa/string.h"
#include "oa/version.h"

namespace oa {
namespace accel {

////////////////////////////////////////////////////////////////////////////////
// addin                                                                      //
////////////////////////////////////////////////////////////////////////////////

addin::addin() : on_auto_free_{[](const xloper12&) {}} {}

addin&
addin::instance()
{
  static addin xll;
  return xll;
}

udf_registry&
addin::udfs()
{
  static udf_registry reg;
  return reg;
}

menu&
addin::menu()
{
  static accel::menu m;
  return m;
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
      str[i] = std::wcout.narrow(wstr[i], '?');  // note: not i18n friendly
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

std::string_view
addin::filename()
{
  static auto name = std::filesystem::path{path()}.filename().string();
  return name;
}

std::string_view
addin::stem()
{
  static auto str = std::filesystem::path{path()}.stem().string();
  return str;
}

addin&
addin::name(std::string str) noexcept
{
  name_ = std::move(str);
  return *this;
}

std::string_view
addin::name() const
{
  // if empty use the default (cannot be called if Excel isn't running)
  if (name_.empty())
    return addin::filename();
  else
    return name_;
}

addin&
addin::on_auto_free(xloper12_callback func)
{
  on_auto_free_ = std::move(func);
  return *this;
}

void
addin::on_auto_free(const xloper12& op) const
{
  on_auto_free_(op);
}

////////////////////////////////////////////////////////////////////////////////
// add-in menu commands                                                       //
////////////////////////////////////////////////////////////////////////////////

/**
 * Displays an informational dialog box about the loaded Accel XLL.
 *
 * This contains the version of Accel used, where the XLL was loaded from, and
 * the date + time the XLL was last compiled.
 */
OA_XLL_EXPORT(int) accel_about()
{
  alert(
    "Accel version: " + std::string{ODIN_VERSION} + "\n"
    "Accel XLL path: " + std::string{addin::path()} + "\n"
    "XLL build date: " + __DATE__ + " " + __TIME__
  );
  return 1;
}

////////////////////////////////////////////////////////////////////////////////
// XLL interface functions                                                    //
////////////////////////////////////////////////////////////////////////////////

/**
 * Excel callback run the first time the add-in manager is used in Excel.
 *
 * This provides the add-in long name to display in the add-in manager dialog.
 */
OA_XLL_EXPORT() xlAddInManagerInfo12(xloper12* op) OA_ACCEL_SAFE()
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
      return oper12{addin::instance().name()};
    else
      return oper12{xlerr::value};
  }();
  // release value back to Excel
  OA_ACCEL_SAFE_RETURN(std::move(res));
}

/**
 * Excel callback run whenever the add-in is activated via the add-in manager.
 *
 * This is not run when Excel starts up and initializes the add-in itself.
 */
OA_XLL_EXPORT(int) xlAutoAdd() noexcept
{
  alert(std::string{addin::instance().name()} + " activated!");
  return 1;
}

/**
 * Excel callback run whenever the add-in is deactivated via add-in manager.
 *
 * This is not run when Excel is shutting down and deactivating add-ins.
 */
OA_XLL_EXPORT(int) xlAutoRemove() noexcept
{
  alert(std::string{addin::instance().name()} + " deactivated!");
  return 1;
}

/**
 * Excel callback to deallocate any `oper12` released back to Excel.
 *
 * This calls the customization hook for user add-ins before `xloper12_free()`.
 */
OA_XLL_EXPORT(void) xlAutoFree12(xloper12* op) noexcept
{
  // pre-free event
  addin::instance().on_auto_free(*op);
  // note: xlbitDLLFree should already be set
  xloper12_free(op);
}

namespace {

/**
 * Register a single Excel UDF.
 *
 * @param res `xlfRegister` return value
 * @param fn UDF to register
 * @param xll_name XLL file name
 */
void add(oper12& res, const udf& fn, const oper12& xll_name)
{
  // create oper12 for all fixed arguments
  std::vector<oper12> args;
  args.push_back(xll_name);             // pxModuleText
  args.emplace_back(fn.export_name());  // pxProcedure
  args.emplace_back(fn.type_text());    // pxTypeText
  args.emplace_back(fn.name());         // pxFunctionText
  args.emplace_back(fn.arg_text());     // pxArgumentText
  args.emplace_back(fn.type());         // pxMacroType
  args.emplace_back(fn.category());     // pxCategory
  // note: Accel doesn't allow registering UDFs with a Ctrl + <key> Excel
  // shortcut to avoid conflicts with existing key bindings. the shortcuts
  // also cannot be unbound unless excel is restarted.
  args.emplace_back(std::string{""});   // pxShortcutText
  args.emplace_back(fn.help_topic());   // pxHelpTopic
  args.emplace_back(fn.help());         // pxFunctionHelp
  // insert argument help text
  for (const auto& arg : fn.args())
    args.emplace_back(arg.help());
  // handle Excel bug where last argument help string has its trailing
  // character chopped off. this is fixed by appending an empty string to the
  // list of arguments and is a known issue fortunately
  args.emplace_back("");
  // create vector of xloper12* for Excel12v()
  std::vector<xloper12*> xl_args;
  for (auto& arg : args)
    xl_args.push_back(arg.value());
  // register
  Excel12v(xlfRegister, res.value(), static_cast<int>(args.size()), xl_args.data());
  // if error, alert, but keep going
  // TODO: can improve this message
  if (res.error()) {
    // error message
    auto err_text = "UDF registration error: Could not register exported " +
      std::string{fn.export_name()} + " as " + std::string{fn.name()};
    // if help is too long (over 255 chars) Excel will fail to register
    if (fn.help().size() > 255)
      err_text += ": UDF help text length " +
        std::to_string(fn.help().size()) + " exceeds Excel limit of 255";
    // emit alert
    alert(err_text);
  }
}

}  // namespace

/**
 * Excel callback required by every XLL for add-in activation.
 */
OA_XLL_EXPORT(int) xlAutoOpen() OA_ACCEL_SAFE(noexcept)
{
  // XLL name
  oper12 xll_name{addin::filename()};
  // xlfRegister return value
  oper12 res;
  // register worksheet functions
  for (const auto& udf : addin::udfs())
    add(res, udf, xll_name);
  // TODO: register menu commands
  // create add-in menu
  if (!worksheet_menu(addin::menu()))
    alert(
      "UDF registration error: Could not create " +
      std::string{addin::filename()} + " add-in menu"
    );
  return 1;
}
// TODO: make this a macro
catch (const std::exception& exc) {
  alert(std::string{"C++ exception: "} + exc.what());
  return 1;
}

/**
 * Excel callback run on XLL deactivation.
 */
OA_XLL_EXPORT(int) xlAutoClose() noexcept
{
  delete_worksheet_menu(addin::menu().clean_name());
  return 1;
}

}  // namespace accel
}  // namespace oa
