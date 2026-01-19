/**
 * @file xl_ops.cpp
 * @author Derek Huang
 * @brief C++ source for `XLCALL.H` type non-member operators
 * @copyright MIT License
 */

#include "oa/accel/xl_ops.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

#include <cstdint>
#include <iostream>     // for std::wcout
#include <ostream>

#include "oa/string.h"  // for oa::hex

////////////////////////////////////////////////////////////////////////////////
// xlref12                                                                    //
////////////////////////////////////////////////////////////////////////////////

bool operator==(const xlref12& a, const xlref12& b) noexcept
{
  return (
    a.rwFirst == b.rwFirst &&
    a.rwLast == b.rwLast &&
    a.colFirst == b.colFirst &&
    a.colLast == b.colLast
  );
}

bool operator!=(const xlref12& a, const xlref12& b) noexcept
{
  return !(a == b);
}

std::ostream& operator<<(std::ostream& out, const xlref12& ref)
{
  // stream top-left cell
  out << "(" << ref.rwFirst << ", " << ref.colFirst << ")";
  // if single-cell reference, done
  if (ref.rwFirst == ref.rwLast && ref.colFirst == ref.colLast)
    return out;
  // otherwise multi-cell reference, so stream bottom-right cell
  return out << " ... (" << ref.rwLast << ", " << ref.colLast << ")";
}

std::ostream& operator<<(std::ostream& out, const xloper12& op)
{
  // switch on type
  switch (op.xltype) {
  // numeric
  case xltypeNum:
    out << op.val.num;
    break;
  // string
  case xltypeStr:
    out << '"';
    // note: size is first element
    for (auto i = 0u; i < static_cast<unsigned>(op.val.str[0]); i++)
      out << std::wcout.narrow(op.val.str[i + 1], '?');
    out << '"';
    break;
  // boolean
  case xltypeBool:
    out << op.val.xbool;
    break;
  // error
  case xltypeErr:
    // TODO: split into separate function
    switch (op.val.err) {
    case xlerrNull:
      out << "#NULL!";
      break;
    case xlerrDiv0:
      out << "#DIV/0!";
      break;
    case xlerrValue:
      out << "#VALUE!";
      break;
    case xlerrRef:
      out << "#REF!";
      break;
    case xlerrName:
      out << "#NAME?";
      break;
    case xlerrNum:
      out << "#NUM!";
      break;
    case xlerrNA:
      out << "#N/A";
      break;
    case xlerrGettingData:
      out << "#WAIT!";
      break;
    }
    break;
  // integer
  case xltypeInt:
    out << op.val.w;
    break;
  // single reference
  case xltypeSRef:
    out << op.val.sref.ref;
    break;
  // multi reference
  case xltypeRef:
    // TODO: kind of duplicates the mref12 operator<<
    out << oa::hex << op.val.mref.idSheet << ": ";
    out << "[";
    for (auto i = 0u; i < op.val.mref.lpmref->count; i++) {
      if (i)
        out << ", ";
      out << op.val.mref.lpmref->reftbl[i];
    }
    out << "]";
    break;
  // array
  // note: always formatted as 2D for now
  case xltypeMulti:
    out << "[";
    for (RW i = 0; i < op.val.array.rows; i++) {
      if (i)
        out << ", ";
      // inner row
      out << "[";
      for (COL j = 0; j < op.val.array.columns; j++) {
        if (j)
          out << ", ";
        out << op.val.array.lparray[i * op.val.array.columns + j];
      }
      out << "]";
    }
    out << "]";
    break;
  // flow
  // TODO: not implemented
  case xltypeFlow:
    out << "<xltypeFlow at 0x" << oa::hex <<
      reinterpret_cast<std::uintptr_t>(&op) << ">";
    break;
  // bigdata
  case xltypeBigData:
    // if data size is zero assume this is holding a handle from Excel
    if (!op.val.bigdata.cbData)
      out << "<xltypeBigData (HANDLE) at 0x" << oa::hex <<
        reinterpret_cast<std::uintptr_t>(op.val.bigdata.h.hdata) << ">";
    // otherwise, indicate data + length
    else
      out << "<xltypeBigData (BYTE*, " << op.val.bigdata.cbData << ") at 0x" <<
        oa::hex <<
        reinterpret_cast<std::uintptr_t>(op.val.bigdata.h.lpbData) << ">";
    break;
  // missing
  case xltypeMissing:
    out << "missing";
    break;
  // nil
  case xltypeNil:
    out << "nil";
    break;
  }
  // chaining
  return out;
}
