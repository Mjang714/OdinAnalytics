<#
    common.ps1

    Common PowerShell support functions.

    This file was originally created to hold some shared PowerShell functions
    frequently used on Windows Github Actions runs. It should be dot-sourced to
    get the functions into the current scope, e.g. with

        . .\common.ps1

    Note:

    By default the execution policy is "Restricted" on Windows desktop machines.
    That is, if you run Get-ExecutionPolicy -List, you will see something like:

                Scope ExecutionPolicy
                ----- ---------------
        MachinePolicy       Undefined
           UserPolicy       Undefined
              Process       Undefined
          CurrentUser       Undefined
         LocalMachine      Restricted

    To enable the current PowerShell session to run + source scripts you should
    use the following Set-ExecutionPolicy command

        Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process -Force

    This will change the Process scope to RemoteSigned to allow scripts.
#>

<#
    .SYNOPSIS
    Translate the CMake VS platform name to a standardized format.

    .DESCRIPTION
    This format is used by the build.bat script to name directories instead of
    using the Pascal case CMake names for cmake -A when using the Visual Studio
    generators. The mapping of the names is as follows:

        Win32 -> x86
        x64 -> x64
        ARM -> arm
        ARM64 -> arm64

    If the input is not one of the above an error is raised.

    .PARAMETER Platform
    CMake platform name, e.g. one of the above

    .EXAMPLE
    $PlatName = OaPlatformTranslate "Win32"
    $PlatName  # should result in "x86" being written sans quotes
#>
function OaPlatformTranslate {
    param ( [string]$Platform )
    switch -CaseSensitive ($Platform) {
        "Win32" { "x86" }
        "x64" { "x64" }
        "ARM" { "arm" }
        "ARM64" { "arm64" }
        default { throw "Unknown platform `"$Platform`"" }
    }
}
