"""oa_time module test.

THis uses unittest to test wrapped oa_test classes, enums, and functions. To
list the test cases in this file, the --list-tests option can be used; this is
mostly useful for test harness integration, e.g. for CTest discovery.

.. codeauthor:: Derek Huang <djh458@stern.nyu.edu>
"""

import unittest

from oa_testutils import test_main
from oa_time import Months, Tenor, Tenors


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


class TestTenors(unittest.TestCase):
    """Test suite for ``oa::time::Tenors`` enum tests."""

    def test_values(self):
        """Test that the enum members have the appropriate values."""
        # member names
        units = ["DAYS", "WEEKS", "MONTHS", "YEARS"]
        # subtest for each
        for unit in units:
            with self.subTest(unit=unit):
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


if __name__ == "__main__":
    test_main(__name__)
