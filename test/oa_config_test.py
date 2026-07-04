"""oa_config module test.

This uses unittest to test wrapped oa_config classes and functions. To list the
test cases in this file, the --list-tests option can be used; this is mostly
useful for test harness integration, e.g. so CTest can discover the tests.

.. codeauthor:: Derek Huang <djh458@stern.nyu.edu>
"""

from pathlib import Path
import unittest

from oa_config import (
    VersionInfo,
    __file__ as oa_config_path,
    data_dir,
    library_dir,
    library_path
)
from oa_testutils import test_main

# location of the oa_config module
_oa_config_dir = Path(oa_config_path).parent


class TestPaths(unittest.TestCase):
    """Test suite for the ``oa::config::`` path helpers."""

    def test_data_dir(self):
        """Test that ``data_dir()` works as expected.

        It should resolve to the directory containing oa_config.
        """
        path = data_dir()
        self.assertEqual(_oa_config_dir, path)

    def test_library_dir(self):
        """Test that ``library_dir()`` works as expected.

        If should resolve to the directory containing oa_config.
        """
        path = library_dir()
        self.assertEqual(_oa_config_dir, path)


class TestVersionInfo(unittest.TestCase):
    """Test suite for ``oa::config::version_info``."""

    @staticmethod
    def config_verinfo() -> VersionInfo:
        """Return a ``VersionInfo`` from ``library_path()``."""
        return VersionInfo(library_path())

    def test_str(self):
        """Test that the ``VersionInfo`` can be created with a ``str``."""
        info = VersionInfo(str(library_path()))

    def test_no_default_ctor(self):
        """Test that the ``VersionInfo`` doesn't expose the default ctor."""
        with self.assertRaises(TypeError):
            _ = VersionInfo()

    def test_no_copy_ctor(self):
        """Test that the ``VersionInfo`` doesn't expose copy/move ctor."""
        with self.assertRaises(TypeError):
            _ = VersionInfo(self.config_verinfo())

    def test_company(self):
        """Test that the ``VersionInfo`` has the expected company string."""
        self.assertEqual("OA Developers", self.config_verinfo().company())

    def test_filename(self):
        """Test that the ``VersionInfo`` has the expected library file name."""
        self.assertEqual(library_path().name, self.config_verinfo().filename())

    def test_version_num(self):
        """Test that the ``VersionInfo`` version number is wrapped correctly."""
        self.assertTrue(isinstance(self.config_verinfo().version_num(), int))


if __name__ == "__main__":
    test_main(__name__)
