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


def enable_param_test(cls: unittest.TestCase) -> unittest.TestCase:
    """unittest ``TestCase`` decorator to enable parametrized tests.

    One missing feature in the ``unittest`` module is the ability to have tests
    parametrized on different test inputs. This is something that is supported
    by some other testing frameworks, e.g. GoogleTest for C++, where one can
    define a test and then separately register tests for different inputs.

    Although ``unittest`` has the ``subTest()`` context manager to enable
    per-iteration subtests in a single test, the overall test method is still
    considered a single test; ``subTest()`` just prevents aborting the test on
    the first ``subTest()`` failure. Therefore, to register a separate test for
    each value-parametrized test input, this class decorator can be used.

    This should be used in conjunction with the ``@parameters`` decorator which
    allows decoration of a ``test_*`` method with its inputs. For example:

    .. code:: python

       @enable_param_test
       class TestAdd(unittest.TestCase):

           # standard unittest test
           def test_one(self):
               self.assertEqual(2, 1 + 1)

           # value-parametrized test rewritten into 4 separate tests
           @parameters((2, 4), (5, 7), (6, 8), (11, 13))
           def test_two(self, input: int, expected: int):
               self.assertEqual(expected, input + 2)

    See the ``@parameters`` docstring for details on how to write such tests.

    Parameters
    ----------
    cls : unittest.TestCase
        Test case class to decorate and rewrite parametrized tests for
    """
    # get current attributes in cls
    # note: copy to freeze any changes although not strictly necessary
    mems = [mem for mem in cls.__dict__]
    # for each attribute
    for mem in mems:
        attr = getattr(cls, mem)
        # param test function has the _param_list member
        if mem.startswith("test_") and hasattr(attr, "_param_list"):
            for i, args in enumerate(getattr(attr, "_param_list")):

                # note: using kwarg capture trick to avoid late binding, which
                # makes it appear as if the last value of each name is used
                def _test(self, test=attr, args=args):
                    # if args is a tuple, unpack
                    if isinstance(args, tuple):
                        test(self, *args)
                    # otherwise pass as a single argument for convenience
                    else:
                        test(self, args)

                # add new test for the given argument[s]
                setattr(cls, f"{mem}_{i}", _test)
            # remove original test function
            delattr(cls, mem)
    return cls


def parameters(*args):
    """unittest test function decorator to enable parametrized test rewriting.

    This decorator should be used to provide the input parameters for a test
    function in a ``unittest.TestCase`` subclass as illustrated in the
    ``@enable_param_test`` docstring. For a test function ``test_func()``, each
    input parameter in ``@parameters`` will result in a new test function
    ``test_func_[i]()``, for values of ``i`` being 0, 1, etc. taking only
    ``self`` as a parameter being added to the test class, with the original
    ``test_func()`` being removed from the test class.

    To support test functions that may require multiple parameters, e.g. an
    input value(s) and an expected output, one can pass a tuple as a parameter,
    which will be unpacked into the arguments of each generated test function.
    This means, however, that if the test function takes a single tuple as a
    parameter, that the tuple needs to be wrapped in another tuple, e.g.

    .. code:: python

       class TestSub(unittest.TestCase):

           @parameters(((1, 0),), ((4, 3),), ((5, 4),))
           def test_one(self, param: tuple[int, int]):
               input, expected = param
               self.assertEqual(expected, input - 1)

    Of course, in this case it would be easier to have two parameters, e.g.

    .. code:: python

       class TestSub(unittest.TestCase):

           @parameters((1, 0), (4, 3), (5, 4))
           def test_one(self, input: int, expected: int):
               self.assertEqual(expected, input - 1)

    One could also choose to use lists for the parameters to avoid unpacking:

    .. code:: python

       class TestSub(unittest.TestCase):

           @parameters([1, 0], [4, 3], [5, 4])
           def test_one(self, param: list[int, int]):
               input, expected = param
               self.assertEqual(expected, input - 1)

    In this case, however, just having more than one parameter is preferred.

    Parameters
    ----------
    *args
        Inputs for each individual test case
    """

    def wrapper(f):
        f._param_list = args
        return f

    return wrapper
