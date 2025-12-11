# OdinAnalytics

<!--
    README.md

    OdinAnalytics README file.

    This file is written in GitHub Flavored Markdown.
-->

Nordic front-office quantitative library sandbox.

This project is inspired by the authors' work experience as front-office strats
at various banks.

## Building

| Platform | CI Status |
| -------- | --------- |
| Linux | [![ci-linux](https://github.com/Mjang714/OdinAnalytics/actions/workflows/linux.yml/badge.svg)](https://github.com/Mjang714/OdinAnalytics/actions/workflows/linux.yml) |
| Windows | [![ci-windows](https://github.com/Mjang714/OdinAnalytics/actions/workflows/windows.yml/badge.svg)](https://github.com/Mjang714/OdinAnalytics/actions/workflows/windows.yml) |

Odin is built with [CMake] >= 3.21 and requires the following:

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
ctest --test-dir build -j$(nproc)
```

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

If C++ unit tests were built they can be run in parallel using CTest with

```shell
ctest --test-dir build -j%NUMBER_OF_PROCESSORS%
```
