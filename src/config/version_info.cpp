/**
 * @file config/version_info.cpp
 * @author Derek Huang
 * @brief C++ source for an OA binary version info access class
 * @copyright MIT License
 */

#include "oa/config/version_info.h"

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winver.h>
#else
#include <elf.h>
#endif  // !defined(_WIN32)

#include <cstddef>
#include <cstdint>
#include <filesystem>
// for std::ifstream to read ELF files
#ifndef _WIN32
#include <fstream>
#endif  // _WIN32
// std::make_unique<T>() to manage temp version info memory
#ifdef _WIN32
#include <memory>
#endif  // _WIN32
// for supporting missing ELF section semantics
#ifndef _WIN32
#include <optional>
#endif  // _WIN32
#include <stdexcept>
#include <string_view>
#include <string>
#include <utility>
// for storing ELF file byte ranges
#ifndef _WIN32
#include <vector>
#endif  // _WIN32

#include "oa/fixed_string.h"
#include "oa/warnings.h"

namespace oa {
namespace config {

// implementation helpers
namespace {

#if defined(_WIN32)
/**
 * Return a view to the `StringFileInfo\040904E4` string table entry.
 *
 * This corresponds to the US English Windows 1252 code page string table. If
 * no corresponding name is available an empty view is returned.
 *
 * @tparam N Length of table entry name + 1
 *
 * @param data `GetFileVersionInfo()` version resource buffer
 * @param name Predefined table entry, e.g. `"ProductName"`, null-terminated
 */
template <std::size_t N>
std::string_view version_string_info_1252(
  const void* data,
  const char (&name)[N]) noexcept
{
  // StringFileInfo block
  fixed_string block{"StringFileInfo\\040904E4\\", name};
  // pointer + length for string info
  char* ptr;
  UINT len;
  // if zero returned then this block is missing
  if (!VerQueryValueA(data, block.data(), reinterpret_cast<void**>(&ptr), &len))
    return {};
  // note: ptr points to a StringTable value which is null-terminated. we use
  // explicit length anyways for security purposes
  return {ptr, len - 1u};
}
#else
/**
 * Check if an ELF file header represents a 64 bit ELF file.
 */
bool is_64(const Elf64_Ehdr& hdr) noexcept
{
  return hdr.e_ident[EI_CLASS] == ELFCLASS64;
}

/**
 * Check if an ELF file header is little endian.
 */
bool is_little_endian(const Elf64_Ehdr& hdr) noexcept
{
  return hdr.e_ident[EI_DATA] == ELFDATA2LSB;
}

/**
 * Check if an ELF file header is big endian.
 */
bool is_big_endian(const Elf64_Ehdr& hdr) noexcept
{
  return hdr.e_ident[EI_DATA] == ELFDATA2MSB;
}

/**
 * Indicate if the ELF file header represents a file of native endianness.
 *
 * We simply first check the host endianness and then check the ELF header.
 */
bool is_native_endian(const Elf64_Ehdr& hdr) noexcept
{
  // reinterpret u16 as 2 bytes to get endianness
  std::uint16_t v = 0xFF00;
  auto vp = reinterpret_cast<const char*>(&v);
  // big-endian if first byte is FF, little-endian if first byte is 00
  return (
    (vp[0] == 0xFF && is_big_endian(hdr)) ||
    (vp[0] == 0x00 && is_little_endian(hdr))
  );
}

/**
 * Indicate if the ELF file haader represents an executable file.
 */
bool is_exe(const Elf64_Ehdr& hdr) noexcept
{
  return hdr.e_type == ET_EXEC;
}

/**
 * Indicate if the ELF file header represents a DSO.
 */
bool is_dyn(const Elf64_Ehdr& hdr) noexcept
{
  return hdr.e_type == ET_DYN;
}

/**
 * Read the ELF file header into an `Elf64_Ehdr`.
 *
 * This function only supports 64-bit ELF objects. If the first 4 bytes are not
 * the ELF magic or the identity is invalid an exception is thrown.
 *
 * @note Only ELF files of *native* endianness are accepted. That is, if the
 *  native system is little-endian, the ELF file must *also* be little-endian.
 *
 * @param fs Input ELF file stream
 */
auto elf_header(std::ifstream& fs)
{
  // note: zeroed for correctness
  Elf64_Ehdr hdr{};
  // helper lambda to populate a member of the Elf64_Ehdr using stream bytes
  // note: using C++20 explicit template parameter list
  auto read = [&fs, &hdr]<typename T>(T Elf64_Ehdr::* mem)
  {
    fs.read(reinterpret_cast<char*>(&(hdr.*mem)), sizeof(T));
  };
  // read header bytes
  read(&Elf64_Ehdr::e_ident);
  // get view of magic + check if actually an ELF file
  std::string_view magic{reinterpret_cast<const char*>(hdr.e_ident), SELFMAG};
  if (magic != ELFMAG)
    throw std::runtime_error{"input stream doesn't point to an ELF file"};
  // validate other e_ident bytes
  // EI_CLASS: must be ELFCLASS64
  if (!is_64(hdr))
    throw std::runtime_error{
      "invalid ELF EI_CLASS " + std::to_string(hdr.e_ident[EI_CLASS])
    };
  // EI_DATA: must be ELFDATA2LSB or ELFDATA2MSB
  if (!is_big_endian(hdr) && !is_little_endian(hdr))
    throw std::runtime_error{
      "invalid ELF EI_DATA " + std::to_string(hdr.e_ident[EI_DATA])
    };
  // EI_VERSION: must be EV_CURRENT
  if (hdr.e_ident[EI_VERSION] != EV_CURRENT)
    throw std::runtime_error{
      "invalid ELF EI_VERSION " + std::to_string(hdr.e_ident[EI_VERSION])
    };
  // EI_OSABI: accept whatever values for now
  // EI_ABIVERSION: accept whatever values for now
  // EI_PAD: ignore padding bytes
  // additional requirement: only operate on ELF objects of native endianness
  if (!is_native_endian(hdr))
    throw std::runtime_error{"only ELF objects of native endianness allowed"};
  // read object file type
  read(&Elf64_Ehdr::e_type);
  // e_type: we only accept ET_DYN, ET_EXEC for now
  if (!is_exe(hdr) && !is_dyn(hdr))
    throw std::runtime_error{"only dynamic/executable ELF objects supported"};
  // read ISA + object file version (must be EV_CURRENT)
  read(&Elf64_Ehdr::e_machine);
  read(&Elf64_Ehdr::e_version);
  if (hdr.e_version != EV_CURRENT)
    throw std::runtime_error{
      "invalid ELF e_version " + std::to_string(hdr.e_version) +
      " != EV_CURRENT (" + std::to_string(EV_CURRENT)
    };
  // read program entry point, program header offset, section header offset
  read(&Elf64_Ehdr::e_entry);
  read(&Elf64_Ehdr::e_phoff);
  read(&Elf64_Ehdr::e_shoff);
  // read flags + header size (should be 0x40)
  read(&Elf64_Ehdr::e_flags);
  read(&Elf64_Ehdr::e_ehsize);
  if (hdr.e_ehsize != 0x40)
    throw std::runtime_error{
      "invalid 64-bit ELF header size " + std::to_string(hdr.e_ehsize) +
      " != expected 64 bytes"
    };
  // read program header table entry size (should be 0x38)
  read(&Elf64_Ehdr::e_phentsize);
  if (hdr.e_phentsize != 0x38)
    throw std::runtime_error{
      "invalid 64-bit ELF program header table entry size " +
      std::to_string(hdr.e_phentsize) + " != expected 56 bytes"
    };
  // read number of entries in program header table
  read(&Elf64_Ehdr::e_phnum);
  // read section header table entry size (should be 0x40)
  read(&Elf64_Ehdr::e_shentsize);
  if (hdr.e_shentsize != 0x40)
    throw std::runtime_error{
      "invalid 64-bit ELF section header table entry size " +
      std::to_string(hdr.e_shentsize) + " != expected 64 bytes"
    };
  // read number of entires in section header table + section name header index
  read(&Elf64_Ehdr::e_shnum);
  read(&Elf64_Ehdr::e_shstrndx);
  // done, return header
  // note: since C++17 copy is elided
  return hdr;
}

/**
 * Return the section header table entry for the section names.
 *
 * The input stream is assumed to point to a valid 64-bit ELF file.
 *
 * @param fs Input ELF file stream
 * @param hdr ELF header for the file associated with the input stream
 */
auto elf_section_name_header(std::ifstream& fs, const Elf64_Ehdr& hdr)
{
  // seek to the appropriate file offset
  fs.seekg(hdr.e_shoff + hdr.e_shstrndx * hdr.e_shentsize);
  // failed
  if (!fs.good())
    throw std::runtime_error{
      "unable to seek to section name header table entry at offset " +
      std::to_string(hdr.e_shoff + hdr.e_shstrndx * hdr.e_shentsize)
    };
  // zeroed section header + read helper
  Elf64_Shdr shdr{};
  auto read = [&fs, &shdr]<typename T>(T Elf64_Shdr::* mem)
  {
    fs.read(reinterpret_cast<char*>(&(shdr.*mem)), sizeof(T));
  };
  // fill
  read(&Elf64_Shdr::sh_name);
  // note: sh_type should be SHT_STRTAB
  read(&Elf64_Shdr::sh_type);
  if (shdr.sh_type != SHT_STRTAB)
    throw std::runtime_error{
      "section name header table entry type is " +
      std::to_string(shdr.sh_type) + " not SHT_STRTAB (" +
      std::to_string(SHT_STRTAB) + ")"
    };
  // note: sh_flags should indicate there are strings but flags are zero
  read(&Elf64_Shdr::sh_flags);
  read(&Elf64_Shdr::sh_addr);
  read(&Elf64_Shdr::sh_offset);
  read(&Elf64_Shdr::sh_size);
  read(&Elf64_Shdr::sh_link);
  read(&Elf64_Shdr::sh_info);
  read(&Elf64_Shdr::sh_addralign);
  read(&Elf64_Shdr::sh_entsize);
  return shdr;
}

/**
 * Return the section header table entry for the named section.
 *
 * The input stream is assumed to point to a valid 64-bit ELF file and the
 * section header entry must correspond to the section with section names.
 *
 * @param fs Input ELF file stream
 * @param hdr ELF header for the file associated with the input stream
 * @param nhdr ELF section header entry corresponding to the section names
 * @param name ELF section name
 */
std::optional<Elf64_Shdr> elf_section_header(
  std::ifstream& fs,
  const Elf64_Ehdr& hdr,
  const Elf64_Shdr& nhdr,
  std::string_view name)
{
  // allocate memory for section names
  std::vector<char> name_data(nhdr.sh_size);
  // jump to section names section + read
  fs.seekg(nhdr.sh_offset);
  fs.read(name_data.data(), name_data.size());
  // seek to section header table
  fs.seekg(hdr.e_shoff);
  // iterate through section headers
  for (auto i = 0u; i < hdr.e_shnum; i++) {
    // section header + read helper
    Elf64_Shdr res{};
    auto read = [&fs, &res]<typename T>(T Elf64_Shdr::* mem)
    {
      fs.read(reinterpret_cast<char*>(&(res.*mem)), sizeof(T));
    };
    // read section fields
    read(&Elf64_Shdr::sh_name);
    read(&Elf64_Shdr::sh_type);
    read(&Elf64_Shdr::sh_flags);
    read(&Elf64_Shdr::sh_addr);
    read(&Elf64_Shdr::sh_offset);
    read(&Elf64_Shdr::sh_size);
    read(&Elf64_Shdr::sh_link);
    read(&Elf64_Shdr::sh_info);
    read(&Elf64_Shdr::sh_addralign);
    read(&Elf64_Shdr::sh_entsize);
    // if matching name + return the section header
    // note: &name_data[res.sh_name] points to a null-terminated string
    if (name == &name_data[res.sh_name])
      return res;
  }
  // no matching header
  return {};
}
#endif  // !defined(_WIN32)

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// version_info::impl_type                                                    //
////////////////////////////////////////////////////////////////////////////////

class version_info::impl_type {
public:
  // implementation of version_info ctor
  impl_type(const std::filesystem::path& path);

  // not copyable or movable
  impl_type(const impl_type&) = delete;
  impl_type(impl_type&&) = delete;

  // dtor
  ~impl_type();

#if defined(_WIN32)
  const char* data_;                    // pointer to VERSIONINFO
  DWORD size_;                          // VERSIONINFO length in bytes
  const VS_FIXEDFILEINFO* fixed_info_;  // fixed-length version info
#else
  std::ifstream in_;                    // ELF file input stream
  Elf64_Ehdr ehdr_;                     // ELF file header
  Elf64_Shdr nhdr_;                     // ELF header for the names section
  std::string company_;                 // oa.company section
  std::string description_;             // oa.description section
  std::string filename_;                // oa.filename section
  std::string product_;                 // oa.product section
  std::string version_;                 // oa.version section
  std::uint64_t version_num_;           // oa.version_num section

  // convenience helper for looking up ELF headers
  std::optional<Elf64_Shdr> header(std::string_view name);

  // read bytes from a valid ELF section header
  std::string section(const Elf64_Shdr& hdr);

  // tag type + global to disambiguate operations that strip trailing nulls
  struct trim_nulls_tag {};
  static constexpr trim_nulls_tag trim_nulls{};

  // read bytes from a valid ELF section header + trim trailing nulls
  std::string section(const Elf64_Shdr& hde, trim_nulls_tag);
#endif  // !defined(_WIN32)
};

// implements version_info construction
#if defined(_WIN32)
version_info::impl_type::impl_type(const std::filesystem::path& path)
{
  // if path doesn't exist, error
  if (!std::filesystem::exists(path))
    throw std::runtime_error{path.string() + " does not exist"};
  // convert to absolute path to disable any LoadLibrary search logic
  auto full_path = std::filesystem::absolute(path);
  // get size in bytes of overall VERSIONINFO
  // note: native Win32 char is wchar_t
  size_ = GetFileVersionInfoSizeW(full_path.c_str(), nullptr);
  if (!size_)
    throw std::system_error{
      static_cast<int>(GetLastError()), std::system_category(),
      "GetFileVersionInfoSizeW()"
    };
  // read file version info data
  // note: use unique_ptr for memory safety
  auto info = std::make_unique<char[]>(size_);
// disable C4267 since DWORD is unsigned long
OA_MSVC_WARNING_PUSH()
OA_MSVC_WARNING_DISABLE(4267)
  if (!GetFileVersionInfoW(full_path.c_str(), 0u, size_, info.get()))
    throw std::system_error{
      static_cast<int>(GetLastError()), std::system_category(),
      "GetFileVersionInfoW()"
    };
OA_MSVC_WARNING_POP()
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
#else
version_info::impl_type::impl_type(const std::filesystem::path& path)
{
  // if path doesn't exist, error
  if (!std::filesystem::exists(path))
    throw std::runtime_error{path.string() + " does not exist"};
  // open in binary mode for reading
  in_ = std::ifstream{path, std::ios::binary};
  // read ELF program header (must be 64-bit) + get section names header
  ehdr_ = elf_header(in_);
  nhdr_ = elf_section_name_header(in_, ehdr_);
  // look up each relevant header
  auto com_hdr = header("oa.company");
  auto desc_hdr = header("oa.description");
  auto fname_hdr = header("oa.filename");
  auto prod_hdr = header("oa.product");
  auto ver_hdr = header("oa.version");
  auto num_hdr = header("oa.version_num");
  // to mimic Windows behavior only num_hdr is required (also because we still
  // allow zero to be a valid version number while other fields can be empty)
  if (!num_hdr)
    throw std::runtime_error{"ELF object missing oa.version_num header"};
  // update string values
  if (com_hdr)
    company_ = section(*com_hdr, trim_nulls);
  if (desc_hdr)
    description_ = section(*desc_hdr, trim_nulls);
  if (fname_hdr)
    filename_ = section(*fname_hdr, trim_nulls);
  if (prod_hdr)
    product_ = section(*prod_hdr, trim_nulls);
  if (ver_hdr)
    version_ = section(*ver_hdr, trim_nulls);
  // reinterpret bytes from oa.version_num section
  // note: we *don't* want to trim nulls as we want the raw bytes
  auto num_str = section(*num_hdr);
  // must have correct size
  if (num_str.size() != sizeof version_num_)
    throw std::runtime_error{
      "oa.version_num header size in bytes " + std::to_string(num_str.size()) +
      " != expected " + std::to_string(sizeof version_num_)
    };
  // otherwise copy bytes in native endianness
  auto num_ptr = reinterpret_cast<char*>(&version_num_);
  for (auto i = 0u; i < sizeof version_num_; i++)
    num_ptr[i] = num_str[i];
}
#endif  // !defined(_WIN32)

version_info::impl_type::~impl_type()
{
// for Windows we need to delete allocated version resource memory
#ifdef _WIN32
  delete[] data_;
#endif  // _WIN32
}

// ELF section header and data helpers
#ifndef _WIN32
std::optional<Elf64_Shdr>
version_info::impl_type::header(std::string_view name)
{
  return elf_section_header(in_, ehdr_, nhdr_, name);
}

std::string
version_info::impl_type::section(const Elf64_Shdr& hdr)
{
  // seek to offset
  in_.seekg(hdr.sh_offset);
  // allocate new string to hold bytes + read
  std::string str(hdr.sh_size, '\0');
  in_.read(str.data(), hdr.sh_size);
  return str;
}

std::string
version_info::impl_type::section(const Elf64_Shdr& hdr, trim_nulls_tag)
{
  // read raw bytes
  auto str = section(hdr);
  // find first null + truncate
  // note: this assumes that the section data is valid, i.e. non-empty
  auto it = str.begin();
  while (it != str.end() && *it)
    it++;
  return {str.begin(), it};
}
#endif  // _WIN32

////////////////////////////////////////////////////////////////////////////////
// version_info                                                               //
////////////////////////////////////////////////////////////////////////////////

version_info::version_info() noexcept {}

version_info::version_info(const std::filesystem::path& path)
  : impl_{new impl_type{path}}
{}

version_info::version_info(version_info&& other) noexcept
{
  from(std::move(other));
}

version_info&
version_info::operator=(version_info&& other) noexcept
{
  destroy();
  from(std::move(other));
  return *this;
}

version_info::~version_info()
{
  destroy();
}

// info getters. on Windows these map to a VerQueryValueA() call while on
// Unix-likes these simply return views to the read ELF section data

std::string_view
version_info::company() const noexcept
{
#if defined(_WIN32)
  return version_string_info_1252(impl_->data_, "CompanyName");
#else
  return impl_->company_;
#endif  // !defined(_WIN32)
}

std::string_view
version_info::description() const noexcept
{
#if defined(_WIN32)
  return version_string_info_1252(impl_->data_, "FileDescription");
#else
  return impl_->description_;
#endif  // !defined(_WIN32)
}

std::string_view
version_info::product() const noexcept
{
#if defined(_WIN32)
  return version_string_info_1252(impl_->data_, "ProductName");
#else
  return impl_->product_;
#endif  // !defined(_WIN32)
}

std::string_view
version_info::version() const noexcept
{
#if defined(_WIN32)
  return version_string_info_1252(impl_->data_, "ProductVersion");
#else
  return impl_->version_;
#endif  // !defined(_WIN32)
}

std::uint64_t
version_info::version_num() const noexcept
{
#if defined(_WIN32)
  // pack most and least significant DWORDs + return
  std::uint64_t ver = impl_->fixed_info_->dwFileVersionMS;
  ver <<= 32;
  ver += impl_->fixed_info_->dwFileVersionLS;
  return ver;
#else
  return impl_->version_num_;
#endif  // !defined(_WIN32)
}

std::string
version_info::version_hex() const
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

void
version_info::from(version_info&& other) noexcept
{
  impl_ = other.impl_;
  other.impl_ = nullptr;
}

void
version_info::destroy() noexcept
{
  if (impl_)
    delete impl_;
}

}  // namespace config
}  // namespace oa
