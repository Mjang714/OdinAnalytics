/**
 * @file config/version_info.h
 * @author Derek Huang
 * @brief C++ header for an OA binary version info access class
 * @copyright MIT License
 */

#ifndef OA_CONFIG_VERSION_INFO_H_
#define OA_CONFIG_VERSION_INFO_H_

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string_view>

#include "oa/config/dllexport.h"

namespace oa {
namespace config {

/**
 * Class representing version info stored in an OA binary object.
 *
 * On Windows this helps control access to the `VERSIONINFO` resource and
 * provides a friendlier way to access the data loaded from a PE32[+] binary.
 * On Unix-likes that use ELF as an object format, constructing this class will
 * read the relevant sections from the ELF shared object or executable.
 *
 * If some string sections are missing from the binary the relevant accessors
 * will return an empty string view. Any problems reading from the given path
 * in the ctor will raise an exception so a construction pattern is:
 *
 * @code{.cc}
 * version_info info;
 * try {
 *  info = path;
 * }
 * catch (const std::exception& exc) {
 *   std::cerr << "Exception: " << exc.what() << std::endl;
 *   // ...
 * }
 * @endcode
 */
class OA_CONFIG_API version_info {
public:
  /**
   * Implementation type.
   *
   * We use PIMPL to hide `Windows.h` and `elf.h` types from the interface. On
   * Windows this manages the version info resource buffer while on Unix-likes
   * using ELF as the object format it manages some auxiliary member values.
   */
  class impl_type;

  /**
   * Default ctor.
   */
  version_info() noexcept;

  /**
   * Ctor.
   *
   * This performs the initial loading of version info bytes from the PE32[+]
   * or ELF file, depending on the platform. If the given path does not exist
   * or the version number section is not available, or any other issue is
   * encountered when reading the file, an exception is thrown.
   *
   * @param path Shared object (.so, .dll) or executable to load
   */
  version_info(const std::filesystem::path& path);

  /**
   * Deleted copy ctor.
   */
  version_info(const version_info&) = delete;

  /**
   * Move ctor.
   */
  version_info(version_info&& other) noexcept;

  /**
   * Move assignment operator.
   */
  version_info& operator=(version_info&& other) noexcept;

  /**
   * Dtor.
   *
   * This frees any implementation memory owned by the `version_info`.
   */
  ~version_info();

  /**
   * Return a view of the company or developer string if present.
   *
   * This typically returns `"OA Developers"`. On Windows, this corresponds to
   * the resource sub-block `StringFileInfo\040904E4\CompanyName`, while on
   * Unix-likes, it corresponds to the `oa.company` ELF object section.
   *
   * If not present an empty view is returned.
   */
  std::string_view company() const noexcept;

  /**
   * Return a view of the description string if present.
   *
   * This should be a short string description of the binary file. On Windows,
   * this is the `StringFileInfo\040904E4\FileDescription` resource, while on
   * Unix-likes, it corresponds to the `oa.description` ELF object section.
   *
   * If not present an empty view is returned.
   */
  std::string_view description() const noexcept;

  /**
   * Return a view of the product name string if present.
   *
   * This represents the overall product suite that the binary is part of. For
   * example, this might return `"OA Software Suite"`. On Windows, this
   * corresponds to the resource `StringFileInfo\040904E4\ProductName`, while
   * on Unix-likes, it corresponds to the `oa.product` ELF object section.
   *
   * If not present an empty view is returned.
   */
  std::string_view product() const noexcept;

  /**
   * Return a view of the full product version string if present.
   *
   * This consists of the full version number as well as any build metadata.
   * For example, "0.1.0-14c2052" for a development build. On Windows, this
   * corresponds to the `StringFileInfo\040904E4\ProductVersion` field, which
   * means the string values will be in the Windows code page 1252 encoding.
   *
   * On Unix-likes this corresponds to the `oa.version` ELF object section.
   *
   * If there is no version string to retrieve then an empty view is returned.
   */
  std::string_view version() const noexcept;

  /**
   * Return the 64-bit numeric version number.
   *
   * This consists of the major, minor, patch, and tweak version numbers, each
   * 16 bits each, packed into an unsigned 64-bit number. On Windows, this
   * corresponds to the `FILEVERSION` numeric version field, while on Unix-
   * likes, it corresponds to the `oa.version_num` ELF object section.
   */
  std::uint64_t version_num() const noexcept;

  /**
   * Return a 16-character hex string representing the numeric version.
   *
   * This formats the 8-byte `numeric_version()` value into a hex string, e.g.
   * for a version 1.2.1.11, something like `"000100020001000b"`.
   */
  std::string version_hex() const;

private:
  impl_type* impl_{};  // implementation pointer

  /**
   * Move-initialize from a `version_info`.
   *
   * On completion, the moved-from `version_info` has no impl pointer.
   */
  void from(version_info&& other) noexcept;

  /**
   * Free any owned memory.
   */
  void destroy() noexcept;
};

}  // namespace config
}  // namespace oa

#endif  // OA_CONFIG_VERSION_INFO_H_
