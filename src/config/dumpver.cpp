/**
 * @file dumpver.cpp
 * @author Derek Huang
 * @brief C++ program to display version info embedded in OA binaries
 * @copyright MIT License
 */

// TODO: support embedding version info in ELF objects

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winver.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <utility>

#include "oa/version.h"

namespace {

// program name and usage
const auto progname = "oa-" + std::filesystem::path{__FILE__}.stem().string();
const auto program_usage = "Usage: " + progname + " [-h] [-v] FILE\n"
  "\n"
  "Print the version information embedded in a OA binary.\n"
  "\n"
  "Currently this only support dumping version info from PE32[+] binaries.\n"
  "\n"
  "Optional arguments:\n"
  "  -h, --help       Print this usage\n"
  "  -v, --version    Print the OA version\n"
  "\n"
  "Required arguments:\n"
  "  FILE             OA binary file with version info to display";

/**
 * Class representing version info stored in an OA binary object.
 *
 * On Windows this helps control access to the `VERSIONINFO` resource and
 * provides a friendlier way to access the data loaded from a PE32[+] binary.
 *
 * @todo May be extended to support reading sections from ELF files.
 */
class version_info {
public:
  /**
   * Default ctor.
   */
  version_info() noexcept = default;

  /**
   * Ctor.
   *
   * If the given path does not exist an exception will be thrown.
   *
   * @param path Shared object or executable to read version info from
   */
  version_info(std::filesystem::path path)
  {
    // if path doesn't exist, error
    if (!std::filesystem::exists(path))
      throw std::runtime_error{path.string() + " does not exist"};
    // convert to absolute path to disable any LoadLibrary search logic
    path = std::filesystem::absolute(path);
    // get size in bytes of overall VERSIONINFO
    // note: native Win32 char is wchar_t
    size_ = GetFileVersionInfoSizeW(path.c_str(), nullptr);
    if (!size_)
      throw std::system_error{
        static_cast<int>(GetLastError()), std::system_category(),
        "GetFileVersionInfoSizeW()"
      };
    // read file version info data
    // note: use unique_ptr for memory safety
    auto info = std::make_unique<char[]>(size_);
    if (!GetFileVersionInfoW(path.c_str(), 0u, size_, info.get()))
      throw std::system_error{
        static_cast<int>(GetLastError()), std::system_category(),
        "GetFileVersionInfoW()"
      };
    // obtain pointer to the fixed info block
    // note: flen is never actually used since we don't really need it
    VS_FIXEDFILEINFO* finfo;
    UINT flen;
    // if return value is zero, no root block (should not happen)
    if (!VerQueryValueA(info.get(), "\\", reinterpret_cast<void**>(&finfo), &flen))
      throw std::runtime_error{"VS_FIXEDFILEINFO missing root block"};
    // done, so update members
    data_ = info.release();
    fixed_info_ = finfo;
  }

  /**
   * Deleted copy ctor.
   */
  version_info(const version_info&) = delete;

  /**
   * Move ctor.
   */
  version_info(version_info&& other) noexcept
  {
    from(std::move(other));
  }

  /**
   * Move assignment operator.
   */
  auto& operator=(version_info&& other) noexcept
  {
    destroy();
    from(std::move(other));
    return *this;
  }

  /**
   * Dtor.
   */
  ~version_info()
  {
    destroy();
  }

  /**
   * Return a view of the company or developer string if present.
   *
   * This typically returns `"OA Developers"`. On Windows, this corresponds to
   * the resource sub-block `StringFileInfo\040904E4\CompanyName`.
   *
   * If not present an empty view is returned.
   */
  std::string_view company() const noexcept
  {
    char* ptr;
    UINT len;
    constexpr auto block = "StringFileInfo\\040904E4\\CompanyName";
    // if zero returned then this block is missing
    if (!VerQueryValueA(data_, block, reinterpret_cast<void**>(&ptr), &len))
      return {};
    // note: ptr points to a StringTable value which is null-terminated. we use
    // explicit length anyways for security purposes
    return {ptr, len - 1u};
  }

  /**
   * Return a view of the description string if present.
   *
   * This should be a short string description of the binary file. On Windows,
   * this is the `StringFileInfo\040904E4\FileDescription` resource.
   *
   * If not present an empty view is returned.
   */
  std::string_view description() const noexcept
  {
    char* ptr;
    UINT len;
    constexpr auto block = "StringFileInfo\\040904E4\\FileDescription";
    // if zero returned then this block is missing
    if (!VerQueryValueA(data_, block, reinterpret_cast<void**>(&ptr), &len))
      return {};
    // note: ptr points to a StringTable value which is null-terminated. we use
    // explicit length anyways for security purposes
    return {ptr, len - 1u};
  }

  /**
   * Return a view of the product name string if present.
   *
   * This represents the overall product suite that the binary is part of. For
   * example, this might return `"OA Software Suite"`. On Windows, this
   * corresponds to the resource `StringFileInfo\040904E4\ProductName`.
   *
   * If not present an empty view is returned.
   */
  std::string_view product() const noexcept
  {
    char* ptr;
    UINT len;
    constexpr auto block = "StringFileInfo\\040904E4\\ProductName";
    // if zero returned then this block is missing
    if (!VerQueryValueA(data_, block, reinterpret_cast<void**>(&ptr), &len))
      return {};
    // note: ptr points to a StringTable value which is null-terminated. we use
    // explicit length anyways for security purposes
    return {ptr, len - 1u};
  }

  /**
   * Return a view of the full product version string if present.
   *
   * This consists of the full version number as well as any build metadata.
   * For example, "0.1.0-14c2052" for a development build. On Windows, this
   * corresponds to the `StringFileInfo\040904E4\ProductVersion` field, which
   * means the string values will be in the Windows code page 1252 encoding.
   *
   * If there is no version string to retrieve then an empty view is returned.
   */
  std::string_view version() const noexcept
  {
    char* ptr;
    UINT len;
    constexpr auto block = "StringFileInfo\\040904E4\\ProductVersion";
    // if zero returned then this block is missing
    if (!VerQueryValueA(data_, block, reinterpret_cast<void**>(&ptr), &len))
      return {};
    // note: ptr points to a StringTable value which is null-terminated. we use
    // explicit length anyways for security purposes
    return {ptr, len - 1u};
  }

  /**
   * Return the 64-bit numeric version number.
   *
   * This consists of the major, minor, patch, and tweak version numbers, each
   * 16 bits each, packed into an unsigned 64-bit number. On Windows, this
   * corresponds to the `FILEVERSION` numeric version field.
   */
  auto version_num() const noexcept
  {
    // pack most and least significant DWORDs + return
    std::uint64_t ver = fixed_info_->dwFileVersionMS;
    ver <<= 32;
    ver += fixed_info_->dwFileVersionLS;
    return ver;
  }

  /**
   * Return a 16-character hex string representing the numeric version.
   *
   * This formats the 8-byte `numeric_version()` value into a hex string, e.g.
   * for a version 1.2.1.11, something like `"000100020001000b"`.
   */
  std::string version_hex() const
  {
    // lowercase hex digits
    constexpr const char digits[] = "0123456789abcdef";
    // numeric version
    auto ver = version_num();
    // convert each of the 8 bytes into 2 hex digits
    std::string str(2 * sizeof ver, '\0');
    for (auto i = 0u; i < sizeof ver; i++) {
      // number of bits to shift right for ith byte to be least significant
      auto shift = 8u * (sizeof ver - i - 1u);
      // mask off first and last 4 bits of byte
      str[i + i] = digits[(ver >> (shift + 4u)) & 0xF];
      str[i + i + 1u] = digits[(ver >> shift) & 0xF];
    }
    return str;
  }

private:
  const char* data_{};                    // pointer to VERSIONINFO
  DWORD size_{};                          // VERSIONINFO length in bytes
  const VS_FIXEDFILEINFO* fixed_info_{};  // fixed-length version info

  /**
   * Move-initialize from a `version_info`.
   *
   * On completion, the moved-from `version_info` is zeroed out.
   */
  void from(version_info&& other) noexcept
  {
    data_ = other.data_;
    size_ = other.size_;
    fixed_info_ = other.fixed_info_;
    other.data_ = nullptr;
    other.size_ = 0u;
    other.fixed_info_ = nullptr;
  }

  /**
   * Free any owned memory.
   */
  void destroy() noexcept
  {
    if (data_)
      delete[] data_;
  }
};

/**
 * Command-line options structure.
 *
 * @param help Print the program usage
 * @param version Print the program version
 * @param path Path to binary file
 */
struct cli_options {
  bool help = false;
  bool version = false;
  std::filesystem::path path;
};

/**
 * Parse incoming command-line options.
 *
 * @param opts Command-line options to fill
 * @param argc Argument count from `main()`
 * @param argc Argument vector from `main()`
 * @returns `true` on success, `false` on error
 */
bool parse_args(cli_options& opts, int argc, char** argv)
{
  for (int i = 1; i < argc; i++) {
    // convenience argument view
    std::string_view arg{argv[i]};
    // -h, --help
    if (arg == "-h" || arg == "--help") {
      opts.help = true;
      return true;
    }
    // -v, --version
    else if (arg == "-v" || arg == "--version")
      opts.version = true;
    // unknown option
    else if (arg.size() && arg[0] == '-') {
      std::cerr << "Error: Unknown option " << arg << std::endl;
      return false;
    }
    // not an option so assign path if not yet asigned
    else if (opts.path.empty())
      opts.path = arg;
    // otherwise too many paths
    else {
      std::cerr << "Error: Too many values provided for FILE argument" <<
        std::endl;
      return false;
    }
  }
  return true;
}

}  // namespace

int main(int argc, char** argv)
{
  // parse command-line options
  cli_options opts;
  if (!parse_args(opts, argc, argv))
    return EXIT_FAILURE;
  // print usage
  if (opts.help) {
    std::cout << program_usage << std::endl;
    return EXIT_SUCCESS;
  }
  // print version and exit
  if (opts.version) {
    std::cout << OA_VERSION << std::endl;
    return EXIT_SUCCESS;
  }
  // load version info from file
  version_info info;
  try {
    info = version_info{argv[1]};
  }
  catch (const std::exception& exc) {
    std::cerr << "Exception: " << exc.what() << std::endl;
    return EXIT_FAILURE;
  }
  // print relevant version info
  std::cout <<
    "product:         " << info.product() << "\n" <<
    "company:         " << info.company() << "\n" <<
    "description:     " << info.description() << "\n" <<
    "version number:  0x" << info.version_hex() << "\n" <<
    "version string:  " << info.version() << "\n" <<
    std::flush;
  return EXIT_SUCCESS;
}
