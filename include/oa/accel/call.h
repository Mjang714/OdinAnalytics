/**
 * @file call.h
 * @author Derek Huang
 * @brief C++ header for functions corresponding to `Excel12()` calls
 * @copyright MIT License
 */

#ifndef OA_ACCEL_CALL_H_
#define OA_ACCEL_CALL_H_

#include <cstdint>
#include <string_view>

namespace oa {
namespace accel {

// forward decl
class menu;

/**
 * Return a handle that corresponds to the top-level Excel window.
 *
 * This should be cast to `HWND` as appropriate with `reinterpret_cast`. Note
 * that depending on which Excel window is on top the return value can change.
 */
std::uintptr_t window();

/**
 * Enum for the different `xlcAlert` styles available.
 *
 * The enumeration values correspond to `xlcAlert` argument values.
 *
 * @note There is no `ok_warn = 3` member because Excel 2016 seems to ignore
 *  that dialog type and simply not pop up a dialog at all.
 */
enum class alert_type : int {
  ok_cancel = 1,  // OK button + Cancel button + question mark icon
  ok_info = 2     // OK button + info icon
};

/**
 * Display an informational dialog box using `xlcAlert`.
 *
 * This uses a type 2 alert box which is informational and only has an "OK"
 * button for the user to click. No value is returned.
 *
 * @note This function is useful for simple alerts but does not offer as much
 *  functionality as the `MessageBoxA()` Win32 function.
 *
 * @param str Message to display in the alert
 */
void alert(std::string_view str);

/**
 * Display an alert dialog box using `xlcAlert` of the specified type.
 *
 * This wraps a call to `xlcAlert` with the given alert dialog type.
 *
 * @param str Message to display in the alert
 * @param type `xlcAlert` alert type
 * @returns `true` if "OK" was clicked, `false` if "Cancel" was clicked
 */
bool alert(std::string_view str, alert_type type);

/**
 * Indicate if the specified worksheet menu exists or not.
 *
 * @param name Menu name sans `'&'` character
 * @returns `true` if menu was found in worksheet menu bar, `false` otherwise
 */
bool worksheet_menu(std::string_view name);

/**
 * Add a worksheet menu with the specified name and commands.
 *
 * All commands and menu separator lines are registered in order. If the menu
 * exists, nothing will be done and `false` will be returned.
 *
 * @note You must ensure that all the menu commands are registered using
 *  `xlfRegister` as otherwise clicking menu buttons pops up an error box.
 *
 * @par
 *
 * @note All command names will have `addin::stem()` + an underscore prefixed
 *  to the name of the exported DLL function used as the command.
 *
 * @param m Menu object to add to the worksheet menu bar
 * @returns `true` if menu registration succeeded, `false` otherwise
 */
bool worksheet_menu(const menu& m);

/**
 * Delete the specified worksheet menu with the specified name.
 *
 * @param name Menu name sans `'&'` character
 * @returns `true` if menu was successfully deleted, `false` otherwise
 */
bool delete_worksheet_menu(std::string_view name);

}  // namespace accel
}  // namespace oa

#endif  // OA_ACCEL_CALL_H_
