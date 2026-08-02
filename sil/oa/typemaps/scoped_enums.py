"""SWIG-specific Python module providing better C++ scoped enum support.

This module should only be used via ``%pythoncode`` directive in SWIG.

.. codeauthor:: Derek Huang <djh458@stern.nyu.edu>
"""

from enum import Enum
import sys


def _make_enum_class(name: str, strip_prefix: str = "") -> None:
    """Convert SWIG-wrapped scoped enum members into an enum class.

    When SWIG wraps C++ scoped enums for Python members with the format
    ``<enum>_<member>`` are added to the SWIG Python wrapper module. These
    members are not very syntactically pretty and lose the scoping associated
    with C++ scoped enums. This function converts any members of the form
    ``<enum>_<member>`` to an ``enum.Enum`` class named ``<enum>``, with all
    the enum members renamed to be upper case, optionally with the enum member
    prefix stripped out, e.g. ``"k"`` or ``"k_"``, depending on how the SWIG
    ``%rename`` directive was used with the enum members.

    This function should be used in a SWIG ``%pythoncode`` block as follows:

    .. code::

       // ensure enum types are in PascalCase for Python
       %rename("%(camelcase)s", %$isenum) "";
       // ensure enum members are in snake_case for Python
       %rename("%(undercase)s", %$isenumitem) "";

       // process header containing scoped enums, e.g. oa::time::Tenors, whose
       // members are wrapped by SWIG as Tenors_* module constants, e.g.
       // Tenors_k_months, due to the above %rename directives
       %include "oa/time/enums.h"

       // use _make_enum_class() to replace Tenors_* members with an enum.Enum
       // named "Tenors" in the current SWIG wrapper module
       %pythoncode %{
       _make_enum_class("Tenors", strip_prefix="k_")
       %}

    .. note::

       The SWIG-wrapped module members for the scoped enum members will be
       removed before the function returns and will no longer be accessible.

    Parameters
    ----------
    name : str
        Enum class name + ``<enum>`` prefix for each ``<enum>_<member>``
    strip_prefix : str, default=""
        Prefix to remove from each C++ scoped enum member after ``%rename``
    """
    # module reference
    mod = sys.modules[__name__]
    # get all SWIG-wrapped members
    enum_mems = [x for x in dir(mod) if x.startswith(f"{name}_")]
    # create enum class as a module attribute
    setattr(
        mod,
        name,
        Enum(
            name,
            [
                # keep only the stripped <member> portion of <enum>_<member>
                (
                    mem.replace(f"{name}_{strip_prefix}", "", 1).upper(),
                    getattr(mod, mem)
                )
                for mem in enum_mems
            ]
        )
    )
    # remove SWIG-wrapped members + return
    for mem in enum_mems:
        delattr(mod, mem)
