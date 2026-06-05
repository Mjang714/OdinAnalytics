/**
 * @file version_elf.cpp
 * @author Derek Huang
 * @brief C++ source for ELF OA version resource embedding
 * @copyright MIT License
 *
 * This C++ source file is the *nix analog to the `version.rc` Windows resource
 * script used to populate the DLL/EXE `VERSIONINFO` resource. Since there is
 * no specific analogue in the ELF format, however, we use compiler attributes
 * to insert string constants at specific ELF data sections that should survive
 * linker object file garbage collection. To view the sections, one can use the
 * standard `objdump` command, or the OA-specific `oa-dumpver`.
 *
 * The following macros should be defined at compile time:
 *
 *    OA_RC_FILE_DESCRIPTION    String literal with descriptive text
 *
 * If possible, the build system should define the following macros at compile
 * time for any per-build-configuration values:
 *
 *    OA_RC_FILE_NAME           String literal file name, e.g. "oa_config.dll"
 *
 * Linking this source's object will add the following ELF object sections:
 *
 *    oa.company                Company name, i.e. "OA Developers"
 *    oa.description            File description
 *    oa.filename               File name at build time
 *    oa.product                Product suite, i.e. "OA Software Suite"
 *    oa.version                Product version string, e.g. "0.1.0-ae1245"
 *    oa.version_num            64-bit major.minor.patch.tweak product version
 *                              number where each component consumes 16 bits
 *
 * The sections map to the Windows `VERSIONINFO` resource fields as follows:
 *
 *    oa.company                StringFileInfo\040904E4\CompanyName
 *    oa.description            StringFileInfo\040904E4\FileDescription
 *    oa.filename               StringFileInfo\040904E4\OriginalFileName
 *    oa.product                StringFileInfo\040904E4\ProductName
 *    oa.version                StringFileInfo\040904E4\ProductVersion
 *    oa.version_num            FILEVERSION, PRODUCTVERSION
 */

#ifdef _WIN32
#error "version_elf.cpp: use version.rc instead on Windows"
#endif  // _WIN32

#include <cstdint>

#include "oa/common.h"   // for OA_STRINGIFY()
#include "oa/version.h"  // for OA_* version macros

// file description. this should be a string literal defined by build system
#ifndef OA_RC_FILE_DESCRIPTION
#define OA_RC_FILE_DESCRIPTION "OA binary artifact"
#endif  // OA_RC_FILE_DESCRIPTION

// 64-bit numeric version macro. currently tweak version is always zero
// TODO: consider putting this in version.h
#define OA_VERSION_NUM \
  (std::uint64_t{OA_MAJOR_VERSION} << 0x30) + \
  (std::uint64_t{OA_MINOR_VERSION} << 0x20) + \
  (std::uint64_t{OA_PATCH_VERSION} << 0x10)

/**
 * Macro defining the section attributes for a version static.
 *
 * @param name Section name to prefix with `oa.` in the ELF object
 */
#define OA_VERSION_SECTION_ATTRIBUTES(name) \
  [[gnu::section("oa." OA_STRINGIFY(name)), gnu::used]]

/**
 * Macro defining a string version section.
 *
 * The character array is null-terminated for convenience.
 *
 * @param name Section name to prefix with `oa.` in the ELF object
 * @param init String literal initializer
 */
#define OA_DEFINE_STRING_VERSION_SECTION(name, init) \
  OA_VERSION_SECTION_ATTRIBUTES(name) \
  static const char OA_CONCAT(oa_, name)[] = {init}

/**
 * Macro defining a 64-bit unsigned integral version section.
 *
 * @param name Section name to prefix with `oa.` in the ELF object
 * @param init Integral initializer value
 */
#define OA_DEFINE_UINT64_VERSION_SECTION(name, init) \
  OA_VERSION_SECTION_ATTRIBUTES(name) \
  static const std::uint64_t OA_CONCAT(oa_, name) = (init)

// embed version info in object file using attributes.
//
// used attribute prevents linker from discarding variables. however, it is
// only applicable to *static*, i.e. *internal linkage*, identifiers, so ensure
// use of the static declaration or an anonymous namespace.
//
// each character array is null-terminated for convenience due to initializaton
// with a string literal. however, for security purposes, the version dumping
// code will take a view of the section bytes except for the trailing null.

OA_DEFINE_STRING_VERSION_SECTION(company, "OA Developers");
OA_DEFINE_STRING_VERSION_SECTION(description, OA_RC_FILE_DESCRIPTION);
#ifdef OA_RC_FILE_NAME
OA_DEFINE_STRING_VERSION_SECTION(filename, OA_RC_FILE_NAME);
#endif  // OA_RC_FILE_NAME
OA_DEFINE_STRING_VERSION_SECTION(product, "OA Software Suite");
OA_DEFINE_STRING_VERSION_SECTION(version, OA_VERSION);
OA_DEFINE_UINT64_VERSION_SECTION(version_num, OA_VERSION_NUM);
