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

You can verify the installation by using the `oa-config` configuration tool:

```bash
/opt/OdinAnalytics/bin/oa-config info
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

You can verify the installation by using the `oa-config` configuration tool:

```shell
C:\OdinAnalytics\bin\oa-config info
```

To "activate" the installation by modifying your user `PATH`, use:

```shell
C:\OdinAnalytics\bin\oa-config activate
```

See the corresponding sections for details on
[per-component installation](#per-component-installation) and
[activation](#activation).

## Per-Component Installation

<!-- note: [!NOTE] only possible with GitHub Flavored Markdown -->

> [!NOTE]
>
> The Development component is incomplete and only provides a few C++ headers
> for downstream consumption. Future work involves migrating some headers into
> a namespaced include structure for downstream consumption.

Odin supports per-component installation via [`cmake --install`][cmake_install]
to enable more fine-grained control over what needs to be installed. Currently
the list of available install components is the following:

Name | Description
---- | -----------
Runtime | Required runtime libraries, tools, and static data
Development | Optional add-on C++ development headers + CMake config files

[cmake_install]: https://cmake.org/cmake/help/latest/manual/cmake.1.html#install-a-project

The Runtime component is always required as it includes ``oa-config``, all Odin
libraries, static data required by the libraries, and on Windows, the `oxl`
Excel add-in. For example, after building 64-bit Release artifacts in
`build_windows_x64`, one can install just the Runtime component into
`C:\OdinAnalytics` with:

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

## Activation

On Windows, the `oa-config` configuration tool provides the `activate`
sub-command that is used to "activate" either the installation the `oa-config`
executable belongs to or another installation rooted elsewhere. "Activation"
involves modifying the current user's `PATH` registry value so that the
specified installation's `bin` directory is part of `PATH` while removing `bin`
directories from other Odin installations to avoid conflicts. This facilitates
use of the `oxl` XLL add-in from Excel, as after activation, Excel can be
launched "normally" from the taskbar with the ability to appropriately locate
any Odin DLLs required by `oxl` for its initial and subsequent loads.

In general, this is a useful feature for integrating with GUI applications that
may use Odin libraries, and enables the relevant `oa-config` and `oa-dumpver`
binaries to be invoked directly as commands in a command shell.

`oa-config` also provides the opposite `deactivate` sub-command that removes
all Odin `bin` directories from the current user's `PATH` to ensure
subsequently launched applications cannot locate any Odin DLLs.
