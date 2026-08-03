"""oa_time module test.

THis uses unittest to test wrapped oa_test classes, enums, and functions. To
list the test cases in this file, the --list-tests option can be used; this is
mostly useful for test harness integration, e.g. for CTest discovery.

.. codeauthor:: Derek Huang <djh458@stern.nyu.edu>
"""

import unittest

from oa_testutils import test_main
from oa_time import Date, Months, Tenor, Tenors


class TestMonths(unittest.TestCase):
    """Test suite for ``oa::time::Months`` enum tests."""

    def test_values(self):
        """Test that the enum members have the appropriate values.

        The month values should be ascending from 1 through 12.
        """
        # member names in sentence case
        months = [
            "January",
            "February",
            "March",
            "April",
            "May",
            "June",
            "July",
            "August",
            "September",
            "October",
            "November",
            "December"
        ]
        # subtest for each
        for i, month in enumerate(months):
            with self.subTest(value=i + 1, month=month):
                self.assertEqual(i + 1, Months[month.upper()].value)


def enable_param_test(cls: unittest.TestCase) -> unittest.TestCase:
    """unittest ``TestCase`` decorator to enable parametrized tests.

    TODO: finsh documenting
    """
    # get current attributes in cls (need actual copy)
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

                setattr(cls, f"{mem}_{i}", _test)
            # remove original test function
            delattr(cls, mem)
    return cls


def parameters(*args):
    # TODO: add docstring + type hints

    def wrapper(f):
        f._param_list = args
        return f

    return wrapper


@enable_param_test
class TestTenors(unittest.TestCase):
    """Test suite for ``oa::time::Tenors`` enum tests."""

    @parameters("DAYS", "WEEKS", "MONTHS", "YEARS")
    def test_value(self, unit: str):
        """Test that the enum members have the appropriate values.

        Parameters
        ----------
        unit : str
            ``Tenors`` enum member name in Python
        """
        self.assertEqual(unit[0], Tenors[unit].value)


class TestTenor(unittest.TestCase):
    """Test suite for ``oa::time::Tenor`` tests."""

    def test_repr(self):
        """Test that the ``Tenor`` representaton works as expected."""
        ten = Tenor(7, Tenors.DAYS)
        self.assertEqual(repr(ten), 'Tenor("7d")')

    def test_str(self):
        """Test that the ``Tenor`` string representation works as expected."""
        ten = Tenor(10, Tenors.YEARS)
        self.assertEqual(str(ten), "10y")

    def test_hash(self):
        """Test that the ``Tenor`` is hashable.

        In particular, ``hash(ten)`` and ``ten.__hash__()`` must be equal, but
        ``ten.hash()`` will be a larger, different value, as the former require
        values truncated to ``Py_ssize_t`` while ``ten.hash()`` returns
        ``std::size_t``, which is an unsigned type of the same size.
        """
        ten = Tenor(20, Tenors.YEARS)
        self.assertEqual(hash(ten), ten.__hash__())

    def test_count(self):
        """Test that ``ten.count()`` works as expected."""
        ten = Tenor(12, Tenors.WEEKS)
        self.assertEqual(12, ten.count())

    def test_unit(self):
        """Test that ``ten.unit()`` works as expected."""
        ten = Tenor(42, Tenors.DAYS)
        self.assertEqual(Tenors.DAYS, ten.unit())

    def test_string_init(self):
        """Test ``Tenor`` initialization from a string.

        This also indirectly tests that ``Tenor`` equality works.
        """
        ten = Tenor("7d")
        self.assertEqual(Tenor(7, Tenors.DAYS), ten)

    def test_neq(self):
        """Test ``Tenor`` negation."""
        t1 = Tenor(7, Tenors.DAYS)
        t2 = Tenor(-7, Tenors.DAYS)
        self.assertEqual(t2, -t1)

    def test_eq(self):
        """Test ``Tenor`` equality.

        This particularly tests "1w" != "7d" since comparison is done on the
        count and units, not the "actual" calendar time.
        """
        t1 = Tenor(1, Tenors.WEEKS)
        t2 = Tenor(7, Tenors.DAYS)
        self.assertNotEqual(t1, t2)

    def test_add(self):
        """Test ``Tenor`` addition.

        .. note::

           Unlike in C++ the ``Tenor`` addition in Python is not symmetric as
           it's not possible to define an unbound user-defined + overload. We
           would need some kind of integral wrapper type for that.
        """
        t1 = Tenor(16, Tenors.MONTHS)
        t2 = Tenor(24, Tenors.MONTHS)
        self.assertEqual(t2, t1 + 8)

    def test_sub(self):
        """Test ``Tenor`` subtraction."""
        t1 = Tenor(19, Tenors.DAYS)
        t2 = Tenor(8, Tenors.DAYS)
        self.assertEqual(t2, t1 - 11)


@enable_param_test
class TestDate(unittest.TestCase):
    """Test suite for ``oa::time::Date`` tests."""

    def test_default_init(self):
        """Test initialization with no arguments (default init)."""
        date = Date()
        self.assertEqual(0, date.julian())

    def test_ymd_init(self):
        """Test initialization with year, month, day."""
        ymd = (2020, 4, 5)
        date = Date(*ymd)
        # note: when gregorian() is wrapped we can use that instead
        self.assertEqual(ymd, (date.year(), date.month(), date.day()))

    @parameters(
        ("2020-04-05", (2020, 4, 5)),
        ("2030-1-3", (2030, 1, 3)),
        ("2032/5/4", (2032, 5, 4)),
        ("2000/09/14", (2000, 9, 14)),
        ("2345:1:1", (2345, 1, 1)),
        ("1908:11:2", (1908, 11, 2))
    )
    def test_string_init(self, ds: str, ymd: tuple[int, int, int]):
        """Test initialiation from a date string.

        This tests a few different valid year, month, day representations.

        Parameters
        ----------
        ds : str
            Date string, e.g. "2020-05-06", for initialization
        ymd : tuple[int, int, int]
            Expected year, month, and day values
        """
        date = Date(ds)
        self.assertEqual(ymd, (date.year(), date.month(), date.day()))

    def test_julian_init(self):
        """Test initialization from a Julian day number."""
        date = Date(2458913)  # 2020-03-04
        self.assertEqual(2458913, date.julian())


if __name__ == "__main__":
    test_main(__name__)
