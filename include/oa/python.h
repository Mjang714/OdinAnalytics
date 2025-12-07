/**
 * @file python.h
 * @author Derek Huang
 * @brief C++ header for Python support
 * @copyright MIT License
 */

#ifndef OA_PYTHON_H_
#define OA_PYTHON_H_

// required before Python 3.13 to use all PyArg_ParseTuple formats
#ifndef PY_SSIZE_T_CLEAN
#define PY_SSIZE_T_CLEAN
#endif  // PY_SSIZE_T_CLEAN
#include <Python.h>

#include <cstddef>
#include <string_view>
#include <utility>

namespace oa {

/**
 * Macro defining the full Python hex version.
 *
 * This is an implementation of `Py_PACK_FULL_VERSION` added in Python 3.14.
 * See https://docs.python.org/3/c-api/apiabiversion.html for details on what
 * each of the version components means.
 *
 * @note This macro evaluates to exactly a 32-bit unsigned integer.
 *
 * @param major Python major version
 * @param minor Python minor version
 * @param micro Python micro/patch version
 * @param level Python release level, e.g. `0xA`, `0xB`, `0xF`
 * @param serial Python release serial
 */
#define OA_PY_FULL_VERSION(major, minor, micro, level, serial) \
  ( \
    (((major) & 0xFF) << 24) | \
    (((minor) & 0xFF) << 16) | \
    (((micro) & 0xFF) << 8) | \
    (((level) & 0xF) << 4) | \
    ((serial) & 0xF) \
  )

/**
 * Macro defining just the Python major + minor versions.
 *
 * This doesn't correspond to a real `PY_VERSION_HEX` value but is useful for
 * numeric comparisons, e.g. `PY_VERSION_HEX >= OA_PY_VERSION(3, 10)`.
 *
 * @param major Python major version
 * @param minor Python minor version
 */
#define OA_PY_VERSION(major, minor) OA_PY_FULL_VERSION(major, minor, 0, 0, 0)

/**
 * Macro for comparing `PY_VERSION_HEX` to the given major + minor versions.
 *
 * The only valid operators allowed are `<`, `>`, `<=`, and `>=`, e.g.
 *
 * @code{.cc}
 * #if OA_PY_VERSION_IS(>=, 3, 9)
 * // ...
 * #endif  // OA_PY_VERSION_IS(>=, 3, 9)
 * @endcode
 *
 * @param op Binary comparison operator
 * @param major Python major version
 * @param minor Python minor version
 */
#define OA_PY_VERSION_IS(op, major, minor) \
  (PY_VERSION_HEX op OA_PY_VERSION(major, minor))

/**
 * Python object class.
 *
 * This provides RAII reference-counted management for `PyObject*` with both
 * copy (reference increment) and move (no reference increment) semantics.
 */
class py_object {
public:
  /**
   * Default ctor.
   *
   * This represents an unowned/invalid Python object.
   */
  constexpr py_object() noexcept : py_object{nullptr} {}

  /**
   * Ctor.
   *
   * Enables non-explicit `constexpr` construction from `nullptr`.
   */
  constexpr py_object(std::nullptr_t) noexcept : obj_{} {}

  /**
   * Ctor.
   *
   * Takes ownership of the given `PyObject*`.
   *
   * @note Not `constexpr` because "real" `PyObject*` exist only at runtime.
   *
   * @param obj Object to own
   */
  explicit py_object(PyObject* obj) noexcept : obj_{obj} {}

  /**
   * Tag type to select reference increment overloads.
   */
  struct inc_tag {};

  /**
   * Tag global to select reference increment overloads.
   */
  static constexpr inc_tag inc{};

  /**
   * Ctor.
   *
   * Takes ownership of a new strong reference to the `PyObject*`.
   *
   * @param obj Object to share
   */
  py_object(inc_tag, PyObject* obj) noexcept : py_object{obj}
  {
    Py_XINCREF(obj_);
  }

  /**
   * Copy ctor.
   *
   * This increments the reference count of the owned Python object.
   */
  py_object(const py_object& other) noexcept
  {
    from(other);
  }

  /**
   * Copy assignment operator.
   *
   * This increments the reference count of the assigned Python object.
   */
  auto& operator=(const py_object& other) noexcept
  {
    Py_XDECREF(obj_);
    from(other);
    return *this;
  }

  /**
   * Move ctor.
   *
   * This preserves the reference count and takes ownership of the reference.
   */
  py_object(py_object&& other) noexcept
  {
    from(std::move(other));
  }

  /**
   * Move assignment operator.
   *
   * This preserves the reference count and takes ownership of the reference.
   */
  auto& operator=(py_object&& other) noexcept
  {
    Py_XDECREF(obj_);
    from(std::move(other));
    return *this;
  }

  /**
   * Dtor.
   *
   * This uses `Py_XDECREF` to correctly handle `nullptr`.
   */
  ~py_object() noexcept
  {
    Py_XDECREF(obj_);
  }

  /**
   * Return the `PyObject*` pointer.
   */
  constexpr auto obj() const noexcept
  {
    return obj_;
  }

  /**
   * Implicitly convert to `PyObject*`.
   *
   * This is useful for interop with Python C API functions.
   */
  operator PyObject*() const noexcept
  {
    return obj_;
  }

  /**
   * Ctor.
   *
   * Creates a new UTF-8 Python string from a copy of the given range.
   *
   * On error the data pointer will be `nullptr`.
   *
   * @param data Character data pointer
   * @param size Number of characters
   */
  py_object(const char* data, std::size_t size) noexcept
  {
    obj_ = PyUnicode_FromStringAndSize(data, size);
  }

  /**
   * Ctor.
   *
   * Creates a new UTF-8 Python string from a copy of the given string view.
   *
   * On error the data pointer will be `nullptr`.
   *
   * @param view String view
   */
  explicit py_object(std::string_view view) noexcept
    : py_object{view.data(), view.size()}
  {}

  /**
   * Ctor.
   *
   * Creates a new Python Unicode string from a wide character range.
   *
   * On error the data pointer will be `nullptr`.
   *
   * @param data Character data pointer
   * @param size Number of characters
   */
  py_object(const wchar_t* data, std::size_t size) noexcept
  {
    obj_ = PyUnicode_FromWideChar(data, size);
  }

private:
  PyObject* obj_;

  /**
   * Copy-initialize from another instance.
   *
   * This increments the reference count of the Python object is not `nullptr`.
   *
   * @param other Object to initialize from
   */
  void from(const py_object& other) noexcept
  {
    obj_ = other.obj_;
    Py_XINCREF(obj_);
  }

  /**
   * Move-initialize from another instance.
   *
   * This does not increment the reference count of the Python object and sets
   * the `PyObject*` of `other` back to `nullptr`.
   *
   * @param other Object to initialize from
   */
  void from(py_object&& other) noexcept
  {
    obj_ = other.obj_;
    other.obj_ = nullptr;
  }
};

}  // namespace oa

#endif  // OA_PYTHON_H_
