/**
 * @file menu.h
 * @author Derek Huang
 * @brief C++ header for an XLL add-in menu
 * @copyright MIT License
 */

#ifndef OA_ACCEL_MENU_H_
#define OA_ACCEL_MENU_H_

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace oa {
namespace accel {

// forward decl to avoid including addin.h
class addin;

/**
 * Class representing the XLL add-in's own custom menu.
 *
 * Sub-menus are not supported for simplicity so only commands and separator
 * lines added with `separator()` may be added.
 */
class menu {
public:
  /**
   * Class representing a standard menu item tied to a command.
   *
   * All menu commands must return `int` and take no parameters so they have a
   * fixed type text of `"J"` and signature of `int __stdcall ()`. When each
   * menu command is registered by `xlAutoOpen()` the actual command name in
   * Excel will be prefixed by the XLL's file stem.
   *
   * For menu item separators there is no command function name associated with
   * the item and the menu's separator count + 1 will be used to set the "name"
   * of the menu, which will be `"-"` repeated `n` times for separator `n`.
   */
  class item {
  public:
    /**
     * Ctor.
     *
     * @param text Menu item text with optional `&` before alt-key character
     * @param func Name of DLL function to use as command (empty for no action)
     */
    item(std::string text, std::string func = "");

    /**
     * Return the menu item text.
     *
     * This includes the optional embedded `&` that indicates the alt key. Menu
     * item separator line text will be `"-"` repeated `n` times for the `n`th
     * item separator line added to the add-in menu.
     */
    std::string_view text() const noexcept;

    /**
     * Return the name of the exported DLL function used as the menu command.
     *
     * This should be empty for separator lines as they don't run commands.
     */
    std::string_view func() const noexcept;

  private:
    std::string text_;  // menu item text
    std::string func_;  // DLL function export name (pxProcedure)
  };

  using container = std::vector<item>;
  using iterator = typename container::const_iterator;

  /**
   * Ctor.
   *
   * Create a new menu with the specified name. If empty, the value of
   * `addin::name()` will be used during the actual menu registration.
   *
   * By default, a menu item called "About..." will be added that displays the
   * Accel version used, the path the XLL was loaded from, and some other TBD
   * info in a dialog box. One separator line is also added to separate the
   * about command from user-defined menu items.
   *
   * @param name Menu name with optional `&` before alt-key character
   */
  menu(std::string name = "");

  /**
   * Return the menu name.
   *
   * If the menu name is empty `addin::name()` will be returned. Therefore,
   * unless you constructed the menu with a non-empty name, do not call
   * `name()` until Excel has started running.
   */
  std::string_view name() const noexcept;

  /**
   * Return the menu name without any ampersand.
   */
  std::string clean_name() const;

  /**
   * Add a new menu item associated with a command.
   *
   * @note Typically this should be used via the `OA_ACCEL_MENU_ITEM()` macro.
   *
   * @param text Menu item text
   * @param func Menu item DLL function used as command
   */
  menu& command(std::string text, std::string func);

  /**
   * Add a new menu separator line.
   */
  menu& separator();

  /**
   * Return an iterator to the first item in the menu.
   */
  iterator begin() const noexcept;

  /**
   * Return an iterator to one past the last item in the menu.
   */
  iterator end() const noexcept;

  /**
   * Return the number of menu items including separator lines.
   */
  std::size_t size() const noexcept;

  /**
   * Return the number of separator line items in the menu.
   */
  std::size_t separators() const noexcept;

  /**
   * Return a reference to the parent addin instance.
   *
   * This simply returns the singleton `addin::instance()` and does not need to
   * be invoked unless additional `addin` member functions need to be called
   * after constructing the menu with `OA_ACCEL_ADDIN_INSTANCE()`.
   */
  addin& operator()() const;

private:
  std::string name_;          // menu name (empty to use add-in file name)
  std::vector<item> items_;   // menu items
  std::size_t separators_{};  // number of separator items inserted in menu
};

}  // namespace accel
}  // namespace oa

#endif  // OA_ACCEL_MENU_H_
