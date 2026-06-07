/**
 * @file config/module.i
 * @author Derek Huang
 * @brief SWIG C++ module for the config library
 * @copyright MIT License
 */

%define MODULE_DOCSTRING
"SWIG module for the OA configuration library.

This module provides wrappers for oa_config library functions and types."
%enddef  // MODULE_DOCSTRING

%module(docstring=MODULE_DOCSTRING) oa_config

// ensure release Python libraries are linked against
%begin %{
#ifdef _MSC_VER
#define SWIG_PYTHON_INTERPRETER_NO_DEBUG
#endif  // _MSC_VER
%}

%{
#include "oa/config/paths.h"
#include "oa/config/version_info.h"
%}

%include "oa/python.i"

OA_HANDLE_EXCEPTIONS

// ensure all class types are in PascalCase
%rename("%(camelcase)s", %$isclass) "";

// process oa/config/dllexport.h for OA_CONFIG_API
%import "oa/config/dllexport.h"

// ensure oa::config::version_info move + default ctor is ignored in Python
// note: need to use %ignore in namespace for this to work
namespace oa::config {

%ignore version_info::version_info();
%ignore version_info::version_info(version_info&&);
// note: ignored by default but we explicitly ignore to silence the warning
%ignore version_info::operator=(version_info&&);

}  // namespace oa::config

%include "oa/config/paths.h"
%include "oa/config/version_info.h"
