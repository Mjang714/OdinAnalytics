/**
 * @file config/main.cpp
 * @author Derek Huang
 * @brief C++ program to report OA library configuration
 * @copyright MIT License
 *
 * On Windows this program can also "activate" an installed distribution by
 * ensuring the `HKEY_CURRENT_USER\Environment\Path` contains the `bin`
 * directory provided by the current or another specified installation. This
 * functionality requires linking against `advapi32.lib` for `winreg.h` usage.
 */

#ifdef _WIN32
// note: included first to correctly set the default value of _WIN32_WINNT
#include <sdkddkver.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinUser.h>
#include <errhandlingapi.h>
#include <processenv.h>
#include <winreg.h>
#endif  // _WIN32

#ifdef _WIN32
#include <algorithm>
#endif  // _WIN32
#include <climits>
#include <cstdlib>
#ifdef _WIN32
#include <cstring>                   // std::memcpy()
#endif  // _WIN32
#include <exception>
#include <filesystem>
#include <iostream>
#ifdef _WIN32
#include <list>
#endif  // _WIN32
#include <string>
#include <string_view>
#include <system_error>
#ifdef _WIN32
#include <type_traits>               // std::type_identity_t<>
#endif  // _WIN32
#include <utility>

#include "oa/config/paths.h"
#ifdef _WIN32
#include "oa/config/version_info.h"
#endif  // _WIN32
#include "oa/features.h"
#include "oa/scope_exit.h"
#include "oa/version.h"
#include "oa/warnings.h"

namespace {

// program name and usage
const std::string progname = "oa-config";
const auto program_usage = "Usage: " + progname + " [-h] [cmd [OPTIONS...]]\n"
  "\n"
  "OA library configuration program.\n"
  "\n"
  "This prints configuration information that can be used in scripts or for\n"
  "general information about the relevant OA installation.\n"
#ifdef _WIN32
  "\n"
  "On Windows, " + progname + " should also be used to \"activate\" the relevant\n"
  "OA installation, or another specified installation, which will modify the\n"
  "PATH variable for the current user so that OA libraries are visible to\n"
  "downstream applications during lookup of DLLs or executables.\n"
#endif  // _WIN32
  "\n"
  "Options:\n"
  "  -h, --help         Print this usage\n"
  "  -v, --version      Print the OA version\n"
  "\n"
  "Commands:\n"
  "  info               Print information on the OA installation. The version,\n"
  "                     architecture, ABI, and available components are listed."
#ifdef _WIN32
  "\n\n"
  "  activate [PATH]    Activate the specified OA installation by modifying\n"
  "                     the user's PATH registry entry. If specified, PATH\n"
  "                     must be the root of an OA installation. Otherwise,\n"
  "                     the currently used installation is [re]activated.\n"
  "\n"
  "  deactivate         Deactivate the active OA installation (if any)"
#endif  // _WIN32
  ;

// note: gui_mode() not yet used on Linux so GCC warns that it is unused
OA_GNU_WARNING_PUSH()
OA_GNU_WARNING_DISABLE(unused-function)
/**
 * Indicate if the program is running in GUI mode or not.
 *
 * If the program is run in GUI mode, then on Linux, then the POSIX `PWD`
 * environment variable should not be present, while on Windows, then more than
 * one program should be attached to the current console. For example, if the
 * program was double-clicked from Windows Explorer, this function should
 * return `true` to indicate running in GUI mode.
 *
 * @note There is a plan to enable `oa-config` to run with a GUI in the future.
 */
bool gui_mode()
{
#if defined(_WIN32)
  // get number of processes attached to current console
  // note: we've chosen a relatively high limit here
  DWORD pids[32u];
  auto n_procs = GetConsoleProcessList(pids, sizeof pids / sizeof(DWORD));
  // handle error
  if (!n_procs)
    throw std::system_error{
      {static_cast<int>(GetLastError()), std::system_category()},
      "GetConsoleProcessList() failed"
    };
  // if equal to 1, this was launched from Explorer (new console)
  return n_procs == 1u;
#else
  // if no PWD environment variable then no POSIX shell environment
  return !std::getenv("PWD");
#endif  // !defined(_WIN32)
}
OA_GNU_WARNING_POP()

/**
 * Scoped enum for the available configuration commands.
 */
enum class config_cmd {
  none,
  activate,
  deactivate,
  info
};

/**
 * Return the string name for the configuration commands.
 *
 * @param v Configuration command value
 */
constexpr auto to_string(config_cmd v) noexcept
{
  switch (v) {
#define CONFIG_CMD_CASE(x) case config_cmd::x: return #x;
  CONFIG_CMD_CASE(none)
  CONFIG_CMD_CASE(activate)
  CONFIG_CMD_CASE(deactivate)
  CONFIG_CMD_CASE(info)
#undef CONFIG_CMD_CASE
  default:
    return "(unknown)";
  }
}

/**
 * Command-line options for the config program.
 */
class cli_options {
public:
  /**
   * Indicate if program help text should be displayed.
   */
  bool help() const noexcept { return help_; }

  /**
   * Set whether program help text should be displayed.
   */
  auto& help(bool v) noexcept
  {
    help_ = v;
    return *this;
  }

  /**
   * Indicate if OA version string should be displayed.
   */
  bool version() const noexcept { return version_; }

  /**
   * Set whether the OA version should be displayed.
   */
  auto& version(bool v) noexcept
  {
    version_ = v;
    return *this;
  }

  /**
   * Return the requested configuration command.
   */
  auto command() const noexcept { return command_; }

  /**
   * Set the configuration command.
   */
  auto& command(config_cmd cmd) noexcept
  {
    command_ = cmd;
    return *this;
  }

  /**
   * Return the specified installation root to activate.
   *
   * If not set this indicates that the current installation root obtained via
   * `oa::config::install_dir()` should be used. If the latter path is empty,
   * then the current OA distribution has not been installed.
   */
  auto& root() const noexcept { return root_; }

  /**
   * Update the specified installation root to activate.
   */
  auto& root(std::filesystem::path root) noexcept
  {
    root_ = std::move(root);
    return *this;
  }

  /**
   * Compare against another `cli_options` for equality.
   */
  bool operator==(const cli_options&) const noexcept = default;

  /**
   * Indicate whether at least one option or command was selected.
   */
  explicit operator bool() const noexcept
  {
    // note: in C++20 operator!= is synthesized from operator==
    return *this != cli_options{};
  }

private:
  bool help_{};
  bool version_{};
  config_cmd command_{};
  std::filesystem::path root_;
};

#ifdef _WIN32
/**
 * Parse incoming command-line options for the `activate` command.
 *
 * @param opts Command-line options to fill
 * @param argc Argument count for `activate`
 * @param argv Argument vector where `argv[0]` is `"activate"`
 * @returns `true` on success, `false` on error
 */
bool parse_activate_args(cli_options& opts, int argc, char** argv)
{
  // set command
  opts.command(config_cmd::activate);
  // iterate for options
  for (int i = 1; i < argc; i++) {
    // convenience arg view
    std::string_view arg{argv[i]};
    // -h, --help short-circuit
    if (arg == "-h" || arg == "--help") {
      opts.help(true);
      return true;
    }
    // if root is not set, treat arg as path
    else if (opts.root().empty()) {
      // if it doesn't exist or is not a directory, error
      if (!std::filesystem::exists(arg)) {
        std::cerr << "Error [activate]: " << arg << " does not exist" <<
          std::endl;
        return false;
      }
      if (!std::filesystem::is_directory(arg)) {
        std::cerr << "Error [activate]: " << arg << " is not a directory" <<
          std::endl;
        return false;
      }
      // update root
      // note: we will make this an absolute path later
      opts.root(arg);
    }
    // otherwise, root already set
    else {
      std::cerr << "Error [activate]: More than one install root provided" <<
        std::endl;
      return false;
    }
  }
  // if root not set, use default (may also return empty path)
  if (opts.root().empty())
    opts.root(oa::config::install_dir());
  // empty, no activation possible
  if (opts.root().empty()) {
    std::cerr << "Error [activate]: No installation root available" << std::endl;
    return false;
  }
  // if no oa-config in bin directory of other root, invalid installation
  if (!std::filesystem::exists(opts.root() / "bin" / (progname + ".exe"))) {
    std::cerr << "Error [activate]: Invalid OA installation root " <<
      opts.root().string() << std::endl;
    return false;
  }
  // use absolute path for root + return
  opts.root(std::filesystem::absolute(opts.root()));
  return true;
}

// MSVC emits C4702 as the i post-increment will never be evaluated
OA_MSVC_WARNING_PUSH()
OA_MSVC_WARNING_DISABLE(4702)
/**
 * Parse incoming command-line options for the `deactivate` command.
 *
 * @param opts Command-line options to fill
 * @param argc Argument count for `deactivate`
 * @param argv Argument vector where `argv[0]` is `"deactivate"`
 * @returns `true` on success, `false` on error
 */
bool parse_deactivate_args(cli_options& opts, int argc, char** argv)
{
  // set command
  opts.command(config_cmd::deactivate);
  // iterate for options
  for (int i = 1; i < argc; i++) {
OA_MSVC_WARNING_POP()
    // convenience arg view
    std::string_view arg{argv[i]};
    // -h, --help short-circuit
    if (arg == "-h" || arg == "--help") {
      opts.help(true);
      return true;
    }
    // unknown option
    else {
      std::cerr << "Error [deactivate]: Unknown option " << arg << std::endl;
      return false;
    }
  }
  // success
  return true;
}
#endif  // _WIN32

// MSVC emits C4702 as the i post-increment will never be evaluated
OA_MSVC_WARNING_PUSH()
OA_MSVC_WARNING_DISABLE(4702)
/**
 * Parse incoming command-line options for the `info` command.
 *
 * @note Currently `info` doesn't take any options.
 *
 * @param opts Command-line options to fill
 * @param argc Argument count for `info`
 * @param argv Argument vector where `argv[0]` is `"info"`
 * @returns `true` on success, `false` on error
 */
bool parse_info_args(cli_options& opts, int argc, char** argv)
{
  // set command
  opts.command(config_cmd::info);
  // iterate for options
  for (int i = 1; i < argc; i++) {
OA_MSVC_WARNING_POP()
    // convenience arg view
    std::string_view arg{argv[i]};
    // -h, --help short-circuit
    if (arg == "-h" || arg == "--help") {
      opts.help(true);
      return true;
    }
    // unknown option
    else {
      std::cerr << "Error [info]: Unknown option " << arg << std::endl;
      return false;
    }
  }
  // if root not set, use default (may also return empty path)
  if (opts.root().empty())
    opts.root(oa::config::install_dir());
  // success
  return true;
}

/**
 * Parse incoming command-line options.
 *
 * @param opts Command-line options to fill
 * @param argc Argument count from `main()`
 * @param argv Argument vector from `main()`
 * @returns `true` on success, `false` on error
 */
bool parse_args(cli_options& opts, int argc, char** argv)
{
  for (int i = 1; i < argc; i++) {
    // convenience arg view
    std::string_view arg{argv[i]};
    // -h, --help
    // note: when -h, --help option is seen, parsing is short-circuited
    if (arg == "-h" || arg == "--help") {
      opts.help(true);
      return true;
    }
    // -v, --version
    else if (arg == "-v" || arg == "--version")
      opts.version(true);
#ifdef _WIN32
    // activate command
    else if (arg == "activate")
      return parse_activate_args(opts, argc - i, argv + i);
    // deactivate command
    else if (arg == "deactivate")
      return parse_deactivate_args(opts, argc - i, argv + i);
#endif  // _WIN32
    // info command
    else if (arg == "info")
      return parse_info_args(opts, argc - i, argv + i);
    // unknown option or command
    else {
      std::cerr << "Error: Unknown command or option " << arg << std::endl;
      return false;
    }
  }
  // success
  return true;
}

#ifdef _WIN32
/**
 * Expand any environment variable references in a wide string path.
 *
 * On Windows environment variable references take the form `%MYVAR%`, for
 * example `%USERPROFILE%`, and can be expanded using the Win32 function
 * `ExpandEnvironmentStringsW()`. We choose to use the wide string version
 * since on Windows `wchar_t` is the native path string character type and
 * because `ExpandEnvironmentStringsA()` documentation says in the output
 * buffer space for an extra character is needed after the null terminator,
 * which is a bit strange, and prevents use of `std::string` as a buffer.
 *
 * @param path Path string with unexpanded environment variable references
 */
auto expand_path(LPCWSTR path)
{
  // get expanded path length (includes terminator)
  // note: use fixed-size small buffer as despite documentation saying lpDst
  // can be nullptr, we instead get a access violation from reading nullptr
  std::wstring exp_path(64, L'\0');
  auto exp_size = ExpandEnvironmentStringsW(path, exp_path.data(), 64);
  if (!exp_size)
    throw std::system_error{
      static_cast<int>(GetLastError()), std::system_category(),
      "ExpandEnvironmentStringsW()"
    };
  // re-allocate new string to exact size + populate
  exp_path = std::wstring(exp_size - 1, L'\0');
  if (!ExpandEnvironmentStringsW(path, exp_path.data(), exp_size))
    throw std::system_error{
      static_cast<int>(GetLastError()), std::system_category(),
      "ExpandEnvironmentStringsW()"
    };
  // return expanded path
  return exp_path;
}

/**
 * Return the `HKEY` associated with `HKEY_CURRENT_USER\Environment`.
 *
 * The registry key has `KEY_QUERY_VALUE` and `KEY_SET_VALUE` permissions and
 * so can be used for querying or setting the value(s).
 */
auto hkcu_environment_key()
{
  // get the HKEY_CURRENT_USER\Environment key
  HKEY key;
  auto err = RegOpenKeyExA(
    HKEY_CURRENT_USER,                // hKey
    "Environment",                    // lpSubKey
    0,                                // ulOptions
    KEY_QUERY_VALUE | KEY_SET_VALUE,  // samDesired
    &key                             // phkResult
  );
  // check errors
  if (err)
    throw std::system_error{err, std::system_category(), "RegOpenKeyA()"};
  return key;
}

/**
 * Return the `Path` value associated with `HKEY_CURRENT_USER\Environment`.
 *
 * The `PATH` string will be returned as a `std::wstring`, in unexpanded form,
 * from the `REG_EXPAND_SZ` stored in the `Path` value.
 *
 * @note The `PATH` string seems to typically have an extra null terminator, so
 *  calling `back()` on the returned `std::wstring` gives `L'\0'`.
 *
 * @param path_key `HKEY_CURRENT_USER\Environment` registry key
 */
auto hkcu_path_string(HKEY path_key)
{
  // get the size of HKEY_CURRENT_USER\Environment\Path (includes terminator)
  // note: using RegGetValueW() since Path should be of type REG_EXPAND_SZ, we
  // we would like the string to be null-terminated for std::wstring, and
  // because on Windows the native path character type is wchar_t
  DWORD path_size;
  auto err = RegGetValueW(
    path_key,                             // hKey
    nullptr,                              // lpSubKey
    L"Path",                              // lpValue
    // note: Path should have REG_EXPAND_SZ type. no expansion
    RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND,  // dwFlags
    // note: we already restricted the type to REG_EXPAND_SZ
    nullptr,                              // pdwType
    nullptr,                              // pvData
    &path_size                            // pcbData
  );
  // check errors
  if (err)
    throw std::system_error{err, std::system_category(), "RegGetValueW()"};
  // allocate + populate string for the PATH value
  // note: can reduce size by 1 since std::wstring buffer is null terminated.
  // we also have to divide path_size by sizeof(wchar_t) since it is in bytes
  std::wstring path_string(path_size / sizeof(WCHAR) - 1u, L'\0');
  err = RegGetValueW(
    path_key,                             // hKey
    nullptr,                              // lpSubKey
    L"Path",                              // lpValue
    RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND,  // dwFlags
    nullptr,                              // pdwType
    path_string.data(),                   // pvData
    // note: if pvData is not nullptr pcbData cannot be nullptr
    &path_size                            // pcbData
  );
  // check errors
  if (err)
    throw std::system_error{err, std::system_category(), "RegGetValueW()"};
  return path_string;
}

/**
 * Update the `Path` value associated with `HKEY_CURRENT_USER\Environment`.
 *
 * The new `PATH` string *must* be null terminated as `RegSetValueExW()`
 * includes the null terminator bytes in its `cbData` parameter.
 *
 * @param path_key `HKEY_CURRENT_USER\Environment` registry key
 * @param path_str Semicolon-separated string for `PATH`
 */
void hkcu_update_path(HKEY path_key, const std::wstring& path_str)
{
  // update PATH
  auto err = RegSetValueExW(
    path_key,                                         // hKey
    L"Path",                                          // lpValueName
    0,                                                // Reserved
    REG_EXPAND_SZ,                                    // dwType
    reinterpret_cast<const BYTE*>(path_str.c_str()),  // lpData
    // note: need to include the size in bytes of the L'\0'
// MSVC emits C4267 due to narrowing from size_t to DWORD
OA_MSVC_WARNING_PUSH()
OA_MSVC_WARNING_DISABLE(4267)
    sizeof(WCHAR) * (path_str.size() + 1)             // cbData
OA_MSVC_WARNING_POP()
  );
  if (err)
    throw std::system_error{err, std::system_category(), "RegSetValueExW()"};
}

/**
 * Return a `std::list` of the wide string directories in the `PATH` string.
 *
 * Each of the directory strings may have unexpanded environment variable
 * references. This function also correctly avoids making any extra trailing
 * null terminators become part of the last directory component.
 *
 * @param path_string `PATH` string from `HKEY_CURRENT_USER\Environment`
 */
auto hkcu_path_list(const std::wstring& path_string)
{
  // iterate through semicolon-separated path strings to build list of paths
  // note: the path strings may have unexpanded environment variable references
  std::list<std::wstring> path_list;
  for (
    auto it = path_string.begin();
    // note: break early if null terminator is encountered. for some reason
    // using RegGetValueW() on PATH gives us a string with an extra null
    it != path_string.end() && *it != L'\0';
  ) {
    // get end of next path string + copy to std::wstring for null termination
    auto end = std::find(it, path_string.end(), L';');
    std::wstring cur_path(it, end);
    // append unexpanded path if not empty + update it
    if (cur_path.size())
      path_list.emplace_back(std::move(cur_path));
    it = end;
    // note: end could one-past-end, so increment only if not
    if (it != path_string.end())
      it++;
  }
  return path_list;
}

/**
 * Return a delimited string from the string values in a `std::list`.
 *
 * This uses the efficient method of determining the total size first before
 * allocating once and then using `std::memcpy()` to copy bytes.
 *
 * @tparam C Character type
 *
 * @param list List of values
 * @param delim Delimiter between each value
 */
template <typename C>
auto to_delimited_string(
  const std::list<std::basic_string<C>>& list,
  std::type_identity_t<C> delim)
{
  // determine total number of characters needed (including delimiter)
  std::size_t size = 0u;
  for (auto it = list.begin(); it != list.end(); it++) {
    if (it != list.begin())
      size += 1u;
    size += it->size();
  }
  // allocate string + get pointer to data for std::memcpy()
  std::basic_string<C> str(size, C{});
  auto out = str.data();
  // copy elements
  for (auto it = list.begin(); it != list.end(); out += it->size(), it++) {
    if (it != list.begin())
      *out++ = delim;
    std::memcpy(out, it->c_str(), sizeof(C) * it->size());
  }
  // done
  return str;
}

/**
 * Activate the OA installation specified.
 *
 * `opts.root()` must be a valid OA installation directory.
 *
 * @param opts Command-line options
 * @returns `EXIT_SUCCESS`
 */
int activate(const cli_options& opts)
{
  // get the HKEY_CURRENT_USER\Environment key + add scoping guard
  auto path_key = hkcu_environment_key();
  oa::scope_exit path_key_g{[path_key] { RegCloseKey(path_key); }};
  // get the unexpanded string associated with the PATH value
  auto path_string = hkcu_path_string(path_key);
  // get list of paths from semicolon-separated path strings
  // note: the path strings may have unexpanded environment variable references
  auto path_list = hkcu_path_list(path_string);
  // start looking for existing active OA installations. these directory paths
  // need to be removed from PATH to prevent use of the wrong installation. we
  // also need to expand each path string for correct path matching
  for (auto it = path_list.begin(); it != path_list.end();) {
    // create a new path object from the *expanded* path
    std::filesystem::path path{expand_path(it->c_str())};
    // check if directory is another installation's bin directory by looking
    // for the presence of oa-config.exe, and if so, remove this path
    if (std::filesystem::exists(path / (progname + ".exe")))
      it = path_list.erase(it);
    // not another installation
    else {
      // if the path does not exist, print an informational message. we can't
      // tell if this is a bad OA installation root or some other PATH
      // directory, so we just tell the user what we've found
      if (!std::filesystem::is_directory(path))
        std::cout << "Warning: " << path.string() <<
          " is not an extant directory" << std::endl;
      // advance
      it++;
    }
  }
  // append opts.root() / bin to list + convert to semicolon-separated string
  path_list.push_back((opts.root() / "bin").wstring());
  path_string = to_delimited_string(path_list, L';');
  // update PATH
  hkcu_update_path(path_key, path_string);
  // get version info for activated oa-config
  oa::config::version_info info{opts.root() / "bin" / (progname + ".exe")};
  std::cout << "[*] OA " << info.version() << " [" << opts.root().string() <<
    "]" << std::endl;
  return EXIT_SUCCESS;
}

/**
 * Deactivate all OA installations.
 *
 * @returns `EXIT_SUCCESS`
 */
int deactivate(const cli_options& /*opts*/)
{
  // get the HKEY_CURRENT_USER\Environment key + add scoping guard
  auto path_key = hkcu_environment_key();
  oa::scope_exit path_key_g{[path_key] { RegCloseKey(path_key); }};
  // get the unexpanded string associated with the PATH value
  auto path_string = hkcu_path_string(path_key);
  // get list of paths from semicolon-separated path strings
  // note: path strings may have unexpanded environment variable references
  auto path_list = hkcu_path_list(path_string);
  // iterate through PATH strings + track count of deactivated installations
  auto n_active = 0u;
  for (auto it = path_list.begin(); it != path_list.end();) {
    // create a new path object from the *expanded* path
    std::filesystem::path path{expand_path(it->c_str())};
    // oa-config.exe in the directory so treat as an existing installation
    if (std::filesystem::exists(path / (progname + ".exe"))) {
      // erase from list + update active count
      it = path_list.erase(it);
      n_active++;
      // print info from the installation's oa-config.exe
      oa::config::version_info info{path / (progname + ".exe")};
      // note: use parent_path() since the path is the bin directory
      std::cout << "[ ] OA " << info.version() << " [" <<
        path.parent_path().string() << "]" << std::endl;
    }
    // not another installation
    else {
      // if the path does not exist, print an informational message
      if (!std::filesystem::is_directory(path))
        std::cout << "Warning: " << path.string() <<
          " is not an extant directory" << std::endl;
      // advance
      it++;
    }
  }
  // convert to semicolon-separated string + update PATH
  path_string = to_delimited_string(path_list, L';');
  hkcu_update_path(path_key, path_string);
  // courtesy message if there were no active installations
  if (!n_active)
    std::cout << "No active OA installations" << std::endl;
  return EXIT_SUCCESS;
}
#endif  // _WIN32

/**
 * Print information about the OA installation.
 *
 * The format will look something like the following:
 *
 * @code
 * OA version:       0.1.0-728e52a
 * OA architecture:  64-bit
 * OA compiler ABI:  MSVC C++ ABI
 * OA install root:  <install path>
 * OA components:    Development, Runtime
 * @endcode
 *
 * @param opts Command-line options
 * @returns `EXIT_SUCCESS`
 */
int print_info(const cli_options& opts)
{
  // get install root as a string
  auto install_root = [&opts]() -> std::string
  {
    if (opts.root().empty())
      return "NOT INSTALLED";
    else
      return opts.root().string();
  }();
  // installed components as string
  auto install_comps = [&opts]() -> std::string
  {
    if (opts.root().empty())
      return "NOT INSTALLED";
    // note: Runtime always available
    if (std::filesystem::is_directory(opts.root() / "include"))
      return "Development, Runtime";
    else
      return "Runtime";
  }();
  // print information
  std::cout <<
    "OA version:       " OA_VERSION "\n" <<
    "OA architecture:  " << sizeof(void*) * CHAR_BIT << "-bit\n" <<
    "OA compiler ABI:  " <<
#if OA_HAS_ITANIUM_ABI
      "Itanium C++ ABI"
#elif defined(_MSC_VER)
      "MSVC C++ ABI"
#else
      "(unknown)"
#endif  // !OA_HAS_ITANIUM_ABI && !defined(_MSC_VER)
      "\n" <<
    "OA install root:  " << install_root << "\n" <<
    "OA components:    " << install_comps << "\n" <<
    std::flush;
  return EXIT_SUCCESS;
}

}  // namespace

int main(int argc, char** argv) try
{
  // parse command-line arguments
  cli_options opts;
  if (!parse_args(opts, argc, argv))
    return EXIT_FAILURE;
// on Windows in GUI mode hide the console window that is displayed. do *not*
// use FreeConsole() as otherwise the next call to gui_mode() will error as the
// console handle associated with the process will be invalid
#ifdef _WIN32
  if (gui_mode())
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif  // _WIN32
// on Windows, we have the option to use GUI. if run from Explorer, use GUI
// mode, although currently we haven't yet settled on a design
#ifdef _WIN32
  if (gui_mode()) {
    // launch message box + handle error
    auto ret = MessageBoxA(
      nullptr,
      "GUI mode not yet implemented!\n"
      "OA version: " OA_VERSION,
      progname.c_str(),
      MB_OK | MB_ICONINFORMATION | MB_DEFBUTTON1 | MB_SETFOREGROUND
    );
    if (!ret)
      throw std::system_error{
        static_cast<int>(GetLastError()), std::system_category(),
        "MessageBoxA()"
      };
    // success
    return EXIT_SUCCESS;
  }
#endif  // _WIN32
  // otherwise, print usage if requested or if no options given
  if (!opts || opts.help()) {
    std::cout << program_usage << std::endl;
    return EXIT_SUCCESS;
  }
  // print just the version
  if (opts.version()) {
    std::cout << OA_VERSION << std::endl;
    return EXIT_SUCCESS;
  }
  // evaluate command
  switch (opts.command()) {
#ifdef _WIN32
  case config_cmd::activate:
    return activate(opts);
  case config_cmd::deactivate:
    return deactivate(opts);
#endif  // _WIN32
  case config_cmd::info:
    return print_info(opts);
  default:
    std::cerr << "Error: Unknown command " << to_string(opts.command()) <<
      std::endl;
    return EXIT_FAILURE;
  }
}
catch (const std::exception& exc) {
  std::cerr << "Exception: " << exc.what() << std::endl;
  return EXIT_FAILURE;
}
