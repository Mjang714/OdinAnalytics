"""OA Python testing helpers.

.. codeauthor:: Derek Huang <djh458@stern.nyu.edu>
"""

from argparse import (
    ArgumentDefaultsHelpFormatter,
    ArgumentParser,
    Namespace,
    RawDescriptionHelpFormatter
)
from importlib import import_module
import sys
from typing import Iterable, Union
import unittest


class HelpFormatter(ArgumentDefaultsHelpFormatter, RawDescriptionHelpFormatter):
    """Custom argparse help formatter.

    This formats into an 80-column width and all other ``__init__()`` default
    arguments are the same as those of the ``argparse.HelpFormatter``.
    """

    def __init__(
        self,
        prog: str,
        indent_increment: int = 2,
        max_help_position: int = 24,
        width: int = 80
    ):
        super().__init__(
            prog,
            indent_increment=indent_increment,
            max_help_position=max_help_position,
            width=width
        )


def list_tests(mod: Union["module", str]) -> list[str]:
    """Return the list of unittest tests in the given module.

    This function counts all the tests in each ``unittest.TestCase`` subclass.

    Parameters
    ----------
    mod : str or ``module``
        Name of module or module object to obtain unittest tests from

    Returns
    -------
    list[str]
        List of "class.name" unittest tests
    """
    if isinstance(mod, str):
        mod = import_module(mod)
    # unittest test case loader
    ldr = unittest.TestLoader()
    # iterate through attributes and collect all TestCase subclasses
    tests = []
    for name in dir(mod):
        attr = getattr(mod, name)
        # skip if not a type
        if not isinstance(attr, type):
            continue
        # if subclass of TestCase, collect tests
        if issubclass(attr, unittest.TestCase):
            tests += [
                f"{attr.__name__}.{test}"
                for test in ldr.getTestCaseNames(attr)
            ]
    return tests


def parse_args(
    args: Iterable[str] | None = None,
    description: str | None = None,
    formatter_class: type | None = HelpFormatter
) -> Namespace:
    """Command-line argument parsing routine for unittest scripts.

    This consumes command-line arguments interpreted by the script using
    ``ArgumentParser.parse_args()``. It provides the following options:

       -l, --list-tests
       -v, --verbose
       -t, --test-case

    Parameters
    ----------
    args : Iterable[str], default=None
        Command-line arguments to parse
    description : str, default=None
        ``ArgumentParser`` description
    formatter_class : type, default=HelpFormatter
        ``ArgumentParser`` formatter class

    Returns
    -------
    argparse.Namespace
    """
    # create parser
    ap = ArgumentParser(description=description, formatter_class=formatter_class)
    ap.add_argument(
        "-l",
        "--list-tests",
        action="store_true",
        help="List the unittest tests available"
    )
    ap.add_argument(
        "-v",
        "--verbose",
        action="count",
        default=1,
        help="Increase the level of unittest verbosity"
    )
    ap.add_argument(
        "-t",
        "--test-case",
        help="Name of a specific test case to run"
    )
    # parse
    return ap.parse_args(args=args)


def test_main(modname: str, args: Iterable[str] | None = None):
    """unittest ``main()`` wrapper.

    This parses the incoming command-line options with ``parse_args()`` and
    passes the parsed arguments to ``unittest.main()`` and exiting the script
    on completion. However, if the -l, --list-tests option is specified, then
    the list of unittest tests is printed and then ``sys.exit(0)`` is called.

    The typical invocation of this function is as follows:

    .. code:: python

       unittest_main(__name__)

    This will use the ``__doc__`` of the ``__name__`` module for the help text
    description printed when passing -h, --help for usage.

    Parameters
    ----------
    modname : str
        Name of the module, typically ``__name__``
    argn : argparse.Namespace, default=None
        Arguments to consume; if ``None``, ``parse_args(**kwargs)`` is run
    """
    # import module
    mod = import_module(modname)
    # parse arguments
    argn = parse_args(args=args, description=mod.__doc__)
    # list tests if specified
    if argn.list_tests:
        print("\n".join(list_tests(modname)))
        sys.exit(0)
    # otherwise run specified test or tests
    unittest.main(
        # prevent consumption of other sys.argv values
        argv=(sys.argv[0],),
        defaultTest=argn.test_case,
        verbosity=argn.verbose
    )
