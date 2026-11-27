# OdinAnalytics

<!--
    README.md

    OdinAnalytics README file.

    This file is written in GitHub Flavored Markdown.
-->

Nordic front-office quantitative library sandbox.

This project is inspired by the authors' work experience as front-office strats
at various banks.

## Build + Install

| Platform | CI Status |
| -------- | --------- |
| Linux | [![ci-linux](https://github.com/Mjang714/OdinAnalytics/actions/workflows/linux.yml/badge.svg)](https://github.com/Mjang714/OdinAnalytics/actions/workflows/linux.yml) |
| Windows | [![ci-windows](https://github.com/Mjang714/OdinAnalytics/actions/workflows/windows.yml/badge.svg)](https://github.com/Mjang714/OdinAnalytics/actions/workflows/windows.yml) |

Odin is built with [CMake] >= 3.23 and requires the following:

* A C++ compiler supporting C++20
* [Boost] >= 1.71
* [Magic Enum] >= 0.8.1

To build the C++ unit tests the following is needed:

* [GoogleTest] >= 1.8.0

To build the `oxl` Excel XLL add-in the following is needed:

* [Excel 2013 XLL SDK]

The C++ unit tests and the Excel add-in will be omitted from the configured
build if their dependencies are not satisfied. In addition, the XLL add-in will
never be built on a non-Windows platform, as XLLs are Windows-specific.

[CMake]: https://cmake.org/cmake/help/latest/
[Boost]: https://www.boost.org/
[Magic Enum]: https://github.com/Neargye/magic_enum
[GoogleTest]: https://github.com/google/googletest
[Excel 2013 XLL SDK]: https://learn.microsoft.com/en-us/office/client-developer/excel/welcome-to-the-excel-software-development-kit

If your dependencies are not installed in system locations the standard CMake
[`<PackageName>_ROOT`][package_root] variables can be defined for
[`find_package`][find_package], e.g. `Boost_ROOT`, `magic_enum_ROOT`,
`GTest_ROOT`, and `XLLSDK_ROOT`. Other search prefix hints like
[`CMAKE_INSTALL_PREFIX`][CMAKE_INSTALL_PREFIX] can also be used as appropriate.

[package_root]: https://cmake.org/cmake/help/latest/variable/PackageName_ROOT.html
[find_package]: https://cmake.org/cmake/help/latest/command/find_package.html
[CMAKE_INSTALL_PREFIX]: https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html

See the corresponding section for details on
[per-component installation](#per-component-installation).

### \*nix

To build Odin on a POSIX system a wrapper `build.sh` script is provided.

For example, to build the Release config with Odin shared libraries, use

```bash
./build.sh -c Release
```

To build Release static libraries instead of shared libraries, use

```bash
./build.sh -c Release -Ca -DBUILD_SHARED_LIBS=OFF
```

Unity builds are supported via [`CMAKE_UNITY_BUILD`][CMAKE_UNITY_BUILD], e.g.
the following works:

```bash
./build.sh -c Release -Ca -DCMAKE_UNITY_BUILD=ON
```

[CMAKE_UNITY_BUILD]: https://cmake.org/cmake/help/latest/variable/CMAKE_UNITY_BUILD.html

Run `./build.sh --help` for a brief usage summary.

If C++ unit tests were built they can be run in parallel using [CTest] with

```bash
./check.sh -p
```

To install Odin into a given install root, e.g. `/opt/OdinAnalytics`, use

```bash
cmake --install build --prefix /opt/OdinAnalytics
```

See the corresponding section for details on
[per-component installation](#per-component-installation).

[CTest]: https://cmake.org/cmake/help/latest/manual/ctest.1.html

### Windows

To build Odin on Windows a wrapper `build.bat` script is provided.

For example, to build the x64 Release config with Odin shared libraries, use

```shell
build -c Release
```

To build x86 Release shared libraries instead x64 shared libraries, use

```shell
build -c Release -a x86
```

To build x64 Release static libraries instead of shared libraries, use

```shell
build -c Release -Ca "-DBUILD_SHARED_LIBS=OFF"
```

Unity builds are supported via `CMAKE_UNITY_BUILD`, e.g. the following works:

```shell
build -c Release -Ca "-DCMAKE_UNITY_BUILD=ON"
```

The double quotes are necessary as without them CMD will get confused by the
`=` and think a variable assignment is being done. For more build options run
`build --help` for a brief usage summary.

If C++ unit tests were built, e.g. for x64 Release libraries, they can be run in
parallel using CTest with

```shell
ctest --test-dir build_windows_x64 -C Release -j%NUMBER_OF_PROCESSORS% --progress
```

To install the Odin x64 Release build to a given install root, e.g.
`C:\OdinAnalytics`, use

```shell
cmake --install build_windows_x64 --prefix C:\OdinAnalytics --config Release
```

See the corresponding section for details on
[per-component installation](#per-component-installation).

## Per-Component Installation

<!-- note: [!NOTE] only possible with GitHub Flavored Markdown -->

> [!NOTE]
>
> The Development component is incomplete and only provides a few C++ headers
> for downstream consumption. Future work involves migrating some headers into
> a proper namespaced include structure.

Odin supports per-component installation via [`cmake --install`][cmake_install]
to enable more fine-grained control over what needs to be installed. Currently
the list of available install components is the following:

Name | Description
---- | -----------
Runtime | Required runtime libraries, tools, and static data
Development | Optional add-on C++ development headers, CMake config files, etc.

[cmake_install]: https://cmake.org/cmake/help/latest/manual/cmake.1.html#install-a-project

The Runtime component is always required as it includes all Odin libraries,
static data required by the libraries, and on Windows, the `oxl` Excel add-in.
For example, after building 64-bit Release artifacts in `build_windows_x64`,
one can install just the Runtime component into `C:\OdinAnalytics` with:

```shell
cmake --install build_windows_x64 --prefix C:\OdinAnalytics --config Release --component Runtime
```

The Development component (currently incomplete) is an optional add-on that
includes C++ headers for downstream developer consumption and any other C++
development components, e.g. a CMake `find_package` package config file,
[PDB][PDB] symbol files, developer tools, etc. For example, after building
64-bit Release artifacts in `build_windows_x64`, one can install the
Development component into `C:\OdinAnalytics` with:

```shell
cmake --install build_windows_x64 --prefix C:\OdinAnalytics --config Release --component Development
```

See the section on [development](#development) for an example of Odin C++
development with CMake.

## Development

> [!NOTE]
>
> The Development component is incomplete and provides only a few C++ headers
> for downstream consumption.

If the Development component is installed via `cmake --install`, Odin provides
some C++ headers for downstream development and CMake `find_package` support.
For example, in a `CMakeLists.txt`, one can do the following:

```cmake
find_package(OA 0.1.0 REQUIRED)
```

As usual, setting `OA_ROOT` or [`CMAKE_PREFIX_PATH`][CMAKE_PREFIX_PATH] can be
used to locate an appropriate installation. Now, suppose we have a simple C++
program that prints the string representation of a date object:

[CMAKE_PREFIX_PATH]: https://cmake.org/cmake/help/latest/variable/CMAKE_PREFIX_PATH.html

```cpp
// date_test.cpp
#include <cstdlib>
#include <iostream>

#include <oa/time/date.h>

int main()
{
  oa::time::Date date{2024, 8, 5};
  std::cout << date << std::endl;
  return EXIT_SUCCESS;
}
```

We can add the following to our `CMakeLists.txt` to compile, link, and add a
[CTest] test for the `date_test` executable:

```cmake
add_executable(date_test date_test.cpp)
target_link_libraries(date_test PRIVATE OA::oa_time)
add_test(NAME date_test COMMAND date_test)
```

Currently, CMake is the only supported build system for C++ development.
