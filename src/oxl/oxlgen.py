"""oxl Excel interface generation tool.

This generates C++ boilerplate for the oxl add-in build.

.. codeauthor:: Michael Jang <Mjang714@gmail.com>
.. codeauthor:: Derek Huang <djh458@stern.nyu.edu>
"""

from argparse import (
    ArgumentDefaultsHelpFormatter,
    ArgumentParser,
    Namespace,
    RawDescriptionHelpFormatter
)
import sys
import os
from pathlib import Path
import sys
from typing import Any, Iterable
import yaml

# path-like typing hint
PathLike = Path | str
# type loosely representing a YAML document
YAMLDocument = dict[str, Any]


class HelpFormatter(ArgumentDefaultsHelpFormatter, RawDescriptionHelpFormatter):
    """Custom argparse help formatting class.

    This maintains the 80-column width that we want. All other ``__init__()``
    defaults are the same as those of the ``argparse.HelpFormatter``.
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


class IncludeResolver:
    """Class that resolves absolute paths for header files.

    It manages a list of include directories that are searched in order.

    Parameters
    ----------
    include_dirs: list[PathLike] | None, default=None
        List of include directory paths. If not specified, ["."] is used
    """

    def __init__(self, include_dirs: list[PathLike] | None = None):
        self.include_dirs = include_dirs if include_dirs else [Path(".")]

    def __call__(self, path: PathLike, relative: bool = False) -> Path:
        """Resolve an absolute path for a given include file.

        If ``relative`` is specified, absolute paths and chopped down to
        relative paths suitable for an ``#include`` directive.

        Parameters
        ----------
        path : PathLike
            Relative or absolute include file path
        relatve : bool, default=False
            Whether or not to keep/convert to a relative path
        """
        # search until found
        for inc_dir in self.include_dirs:
            # assume absolute. if relative, resolve back to absoluate
            inc_candidate = Path(path)
            if not inc_candidate.is_absolute():
                inc_candidate = (Path(inc_dir) / path).resolve()
            # ok, found the file
            if inc_candidate.exists():
                # make relative if necessary
                # note: inc_dir may be relative so we resolve it here
                return (
                    inc_candidate.relative_to(inc_dir.resolve())
                    if relative else inc_candidate
                )
        # can't find it so error
        # note: later versions of Python disallow \ within f-string brackets
        raise RuntimeError(
            f"Unable to find locate header file {path} in:\n  " +
            "\n  ".join(str(inc_dir) for inc_dir in self.include_dirs)
        )


# TODO: change yml_definition to yml_def
#this function creates the the header function for the registration
def header_reg_func(num_func: int, prefix_reg: str) -> list[str]:
    # TODO: enable print in verbose mode
    # print("Generating header file for " + prefix_reg)
    xl_reg_hdr = []
    xl_reg_hdr.append("// This is the registration function header that excel will use expose the function outwards.")
    xl_reg_hdr.append("// This auto generated header registration file that leverages the yaml file.")
    xl_reg_hdr.append("")
    xl_reg_hdr.append("#ifndef WIN32_LEAN_AND_MEAN")
    xl_reg_hdr.append("#define WIN32_LEAN_AND_MEAN")
    xl_reg_hdr.append("#endif  // WIN32_LEAN_AND_MEAN")
    xl_reg_hdr.append("#ifndef NOMINMAX")
    xl_reg_hdr.append("#define NOMINMAX")
    xl_reg_hdr.append("#endif  // NOMINMAX")
    xl_reg_hdr.append("#include <Windows.h>")
    xl_reg_hdr.append("#include <stdlib.h>")
    xl_reg_hdr.append("")
    xl_reg_hdr.append("#include <format>")
    xl_reg_hdr.append("#include <iostream>")
    xl_reg_hdr.append("#include <string>")
    xl_reg_hdr.append("")
    xl_reg_hdr.append("#include \"xlcall.h\"")
    xl_reg_hdr.append("#include \"framewrk.h\"")
    xl_reg_hdr.append("")
    xl_reg_hdr.append("#define " + prefix_reg + "_num_reg_functions_rows "+ str(num_func))
    xl_reg_hdr.append("")
    xl_reg_hdr.append("static const WCHAR* "+ prefix_reg + "_ngFuncs[" + prefix_reg + "_num_reg_functions_rows][12] = {")

    # TODO: enable print in verbose mode
    # print("Finished registration header creation for " + prefix_reg)

    return xl_reg_hdr

def register_function(yml_defintion):
    rfs = header_reg_func(len(yml_defintion["Functions"]), yml_defintion["RegPreFix"])

    for func_desc in yml_defintion["Functions"]:
        args = func_desc["Args"]
        line ="{ L\""
        line += func_desc["ExcelName"]
        line += "\", L\""
        line += "Q"
        if args:
            for arg in args:
                line+=XlRegType(arg["Type"])
        line += "\", L\""
        line += func_desc["ExcelName"]
        line += "\", L\""
        if args:
            for arg in args[:-1]:
                line += arg["Name"]
                line += ","
        if args:
            line += args[-1]["Name"]
        line += "\", L\"1\",L\""
        line += yml_defintion["Category"]
        line += "\",L\"\",L\"\",L\""
        line += func_desc["Help"]
        line += "\",},"
        rfs+=[line]

    rfs+=["};"]
    return rfs

def XlRegType(rt):
    return "Q"

def XlType():
    return "LPXLOPER12"

def try_block():
    auto_gen_try = ["  try"]
    return auto_gen_try

def split_type(return_type):

    if(return_type.find("|") != -1):
        pointer_type = return_type[return_type.find("|") + 1:]
        return_type = return_type[:return_type.find("|")]

    else:
        pointer_type = ""

    return (return_type, pointer_type)

def catch_block():
    catch_block_auto_gen = ["   catch(std::exception& e)"]
    catch_block_auto_gen += ["   {"]
    catch_block_auto_gen += ["       oxl::xl_api::XLoperObj::ConvertToLPXloper(\"ERROR:\" + std::string(e.what()), xloper_result);"]
    catch_block_auto_gen += ["       return xloper_result;"]
    catch_block_auto_gen += ["   }"]
    catch_block_auto_gen += [""]
    catch_block_auto_gen += ["   catch(...)"]
    catch_block_auto_gen += ["   {"]
    catch_block_auto_gen += ["       xloper_result->xltype = xltypeErr;"]
    catch_block_auto_gen += ["       xloper_result->val.err = xlerrValue;"]
    catch_block_auto_gen += ["       return xloper_result;"]
    catch_block_auto_gen += ["   }"]
    return catch_block_auto_gen

def OxlType(arg_type, ptr_type):

    if arg_type == "Double":
        return "double"
    elif arg_type == "Integer":
        return "int"
    elif arg_type == "String":
        return "std::string"
    elif arg_type == "Handle":
        return "std::shared_ptr<"+ ptr_type +">"
    elif arg_type == "Variant":
        return "XLoperObj"
    else:
        print("not a valid input type" + arg_type + " thus will return an empty string" )
        return ""

def create_excel_cpp(
    yml: YAMLDocument,
    resolver: IncludeResolver = IncludeResolver()
) -> list[str]:
    auto_gen = excel_function_header(yml, resolver=resolver)
    for func_desc in yml["Functions"]:
        func_type = func_desc["Type"]
        if(func_type == "Excel"):
            auto_gen += excel_functions_base(func_desc)
        elif(func_type == "Vanilla"):
            auto_gen += vanilla_functions_base(func_desc)
        elif(func_type == "Dictionary"):
            auto_gen += dictionary_functions_base(func_desc)
        auto_gen += ["       return xloper_result;"]
        # create another else here for return type that is Array for the line above
        auto_gen += ["   }"]
        auto_gen += [" "]
        auto_gen += catch_block()
        auto_gen += ["}"]
        auto_gen += [" "]

    return auto_gen


def get_header_deps(
    yml: YAMLDocument,
    resolver: IncludeResolver = IncludeResolver(),
    relative: bool = False
) -> list[Path]:
    """Return a list of header paths required by the YAML interface.

    These include the standard conversion headers as well any oxl-relative
    headers specified in the HeaderFiles field of the YAML input file.

    Parameters
    ----------
    yml : YAMLDocument
        YAML input from an interface input file
    resolver : IncludeResolver, default=IncludeResolver()
        Include path resolver
    relative : bool, default=False
        True to return relative paths for ``#include`` instead

    Returns
    -------
    list[Path]
        List of absolute paths to the specified header files
    """
    # default header paths
    paths = [
        resolver("oxl/xl_api/cache_xl_obj.h", relative=relative),
        resolver("oxl/xl_api/xl_dictionary.h", relative=relative),
        resolver("oxl/xl_api/xloper_converter.h", relative=relative)
    ]
    # add any extra headers. yml["HeaderFiles"] is a list[str]
    # TODO: enable use of additional include directories
    if yml["HeaderFiles"]:
        paths += [
            resolver(path, relative=relative)
            for path in yml["HeaderFiles"]
        ]
    # done
    return paths


def make_depfile_content(
    yml_path: PathLike,
    yml: YAMLDocument,
    resolver: IncludeResolver = IncludeResolver()
) -> str:
    """Return the depfile string content representing the YAML dependencies.

    OutputFile and RegFile are kept as relative paths so that CMake can
    interpret them as relative to the binary directory which would be set via
    this script's -o, --output-directory option.

    The YAML input file and the path to this script itself are always present
    as dependencies for the obvious reasons.

    Parameters
    ----------
    yml_path : PathLike
        Path to the YAML input file
    yml : YAMLDocument
        YAML document read from ``yml_path``

    Returns
    -------
    str
    """
    # main output from OutputFile. YAML path is main input + headers. note that
    # this script itself is *also* a dependency, as if the code generator
    # changes, you probably want to re-run code generation
    # note: __file__ is absolute since Python 3.9
    return "".join(
        [
            f"{yml['OutputFile']}: \\\n"
            f"  {str(Path(yml_path).absolute())} \\\n"
            f"  {__file__}"
        ] + [
            f" \\\n  {str(path)}"
            for path in get_header_deps(yml, resolver=resolver)
        ] + [
            "\n"
        ]
    )


# TODO: document
def excel_function_header(
    yml: YAMLDocument,
    resolver: IncludeResolver = IncludeResolver()
) -> list[str]:
    header = []
    header.append("// This is the registration function cpp that excel will use expose the function outwards.")
    header.append("// This auto generated cpp registration file that leverages the yaml file.")
    header.append("")
    header.append("#ifndef WIN32_LEAN_AND_MEAN")
    header.append("#define WIN32_LEAN_AND_MEAN")
    header.append("#endif  // WIN32_LEAN_AND_MEAN")
    header.append("#ifndef NOMINMAX")
    header.append("#define NOMINMAX")
    header.append("#endif  // NOMINMAX")
    header.append("#include <Windows.h>")
    header.append("")
    header.append("#include <stdlib.h>")
    header.append("")
    header.append("#include <format>")
    header.append("#include <iostream>")
    header.append("#include <string>")
    header.append("")
    header.append("#include \"xlcall.h\"")
    header.append("#include \"framewrk.h\"")
    header.append("")
    # get header dependencies + update header (all relative)
    header += [
        f"#include \"{str(dep)}\""
        for dep in get_header_deps(yml, resolver=resolver, relative=True)
    ]
    # add extra empty line + return
    header.append("")
    return header

def create_cached_handle(yml_definition):
    auto_gen_code = []
    line = "       oxl::xl_api::CachedObjVar cache_obj = oxl::" + yml_definition["OxlName"]+ "("
    argline = " "
    if (yml_definition["Args"]) :
        for arg in yml_definition["Args"]:
            argline += arg["Name"] + "_input,"
        line += argline[:-1]
        line += " );"
    auto_gen_code += [line]
    auto_gen_code += ["       std::string base_name = oxl::xl_api::XlCacheObj::GenHandleStr(cache_obj);"]
    auto_gen_code += ["       auto cached_obj = oxl::xl_api::XlCacheObj(cache_obj, base_name);"]
    auto_gen_code += ["       oxl::xl_api::XLoperObj::ConvertToLPXloper(cached_obj.CacheName(), xloper_result);"]
    return auto_gen_code

def create_generic_xloper(yml_definition):
    auto_gen_code = []
    line = "       auto intermediate_result = oxl::" + yml_definition["OxlName"]+ "("
    argline = " "
    if (yml_definition["Args"]) :
        for arg in yml_definition["Args"]:
            argline += arg["Name"] +"_input,"
        line += argline[:-1]
        line += " );"
    auto_gen_code += [line]
    auto_gen_code += ["       oxl::xl_api::XLoperObj::ConvertToLPXloper(intermediate_result, xloper_result);"]
    return auto_gen_code

def dictionary_functions_base(yml_definition):
    dictionary_code = []
    function_name = yml_definition["ExcelName"]
    function_header = "extern \"C\" __declspec(dllexport) \nLPXLOPER12  "
    function_header += create_function_declaration(function_name, yml_definition)
    dictionary_code = [function_header]
    dictionary_code += ["{"]
    dictionary_code += ["  LPXLOPER12 xloper_result = static_cast<LPXLOPER12> (calloc(1, sizeof(xloper12)));"]
    dictionary_code += try_block()
    dictionary_code += [" {"]
    dictionary_code += ["    oxl::xl_api::XlDictionary dictionary_input;"]
    if(len(yml_definition["Args"]) > 1):
        dictionary_code += ["    if (oxl::xl_api::XLoperObj::IsMulti(" + yml_definition["Args"][0]["Name"]+ "_input ) && (oxl::xl_api::XLoperObj::IsMulti(" + yml_definition["Args"][1]["Name"]+ "_input )))" ]
        dictionary_code += ["    {"]
        dictionary_code += ["         dictionary_input = oxl::xl_api::XLoperObj::LPXloperToDictionary(" + yml_definition["Args"][0]["Name"] + "_input, "+ yml_definition["Args"][1]["Name"] +"_input);"]
        dictionary_code += ["    }"]
        dictionary_code += ["    else if (oxl::xl_api::XLoperObj::IsMulti(" + yml_definition["Args"][0]["Name"]+ "_input))" ]
        dictionary_code += ["    {"]
        dictionary_code += ["         dictionary_input = oxl::xl_api::XLoperObj::LPXloperToDictionary(" + yml_definition["Args"][0]["Name"] + "_input);"]
        dictionary_code += ["    }"]
    else:
        dictionary_code += ["    if (oxl::xl_api::XLoperObj::IsMulti(" + yml_definition["Args"][0]["Name"]+ "_input))" ]
        dictionary_code += ["    {"]
        dictionary_code += ["         dictionary_input = oxl::xl_api::XLoperObj::LPXloperToDictionary(" + yml_definition["Args"][0]["Name"] + "_input);"]
        dictionary_code += ["    }"]
    dictionary_code += ["    else"]
    dictionary_code += ["    {"]
    dictionary_code += ["         if(oxl::xl_api::XLoperObj::IsStr("+  yml_definition["Args"][0]["Name"] + "_input))"]
    dictionary_code += ["         {"]
    dictionary_code += ["            std::string handle_str = oxl::xl_api::XLoperObj::LPXloperToStr("+ yml_definition["Args"][0]["Name"] + "_input);"]
    dictionary_code += ["            std::string key = oxl::xl_api::XlCacheObj::GetKeyFromHandle(handle_str);"]
    dictionary_code += ["            if(oxl::xl_api::XlCacheObj::IsDictionary(key))"]
    dictionary_code += ["            {"]
    dictionary_code += ["              auto cache_variant = oxl::xl_api::XlCacheObj::GetVariant(key);"]
    dictionary_code += ["              dictionary_input = *std::get<std::shared_ptr<oxl::xl_api::XlDictionary>>(cache_variant);"]
    dictionary_code += ["            }"]
    dictionary_code += ["         }"]
    dictionary_code += ["         else"]
    dictionary_code += ["         {"]
    create_dictionary_line = ""
    if(yml_definition["Args"]):
        for arg in yml_definition["Args"]:
            #print(arg["Name"])
            create_dictionary_line +=  "           dictionary_input[\"" + arg["Name"] + "\"] = "
            if(arg["Type"] == "String"):
                create_dictionary_line += "oxl::xl_api::XLoperObj::LPXloperToStr(" + arg["Name"] + "_input); \n"
            elif(arg["Type"] == "LPXLOPER|Date"):
                create_dictionary_line += "oxl::xl_api::XLoperObj::LPXloperToDouble(" + arg["Name"] + "_input); \n"
    dictionary_code += [create_dictionary_line]
    dictionary_code += ["         }"]
    dictionary_code += ["    }"]
    line = "       auto intermediate_result = oxl::" + yml_definition["OxlName"]+ "(dictionary_input);"
    dictionary_code += [line]
    dictionary_code += ["       oxl::xl_api::XLoperObj::ConvertToLPXloper(intermediate_result, xloper_result);"]
    return dictionary_code


def create_function_declaration(name, yml_definition):
    line = name + "("
    if yml_definition["Args"]:
        for argument in yml_definition["Args"]:
            arg_type = argument["Type"]
            (rt, pt) = split_type(arg_type)
            line += XlType()
            line += " "
            line += argument["Name"] + "_input"
            line += ", "
        line = line[:-2]
    line += ")"

    return line


def vanilla_functions_base(yml_definition):
    vanilla_code = []
    name = yml_definition["ExcelName"]
    line = "extern \"C\" __declspec(dllexport) \nLPXLOPER12  "
    line += create_function_declaration(name, yml_definition)
    vanilla_code += [line]
    vanilla_code += ["{ "]
    vanilla_code += ["  LPXLOPER12 xloper_result = static_cast<LPXLOPER12> (calloc(1, sizeof(xloper12)));"]
    vanilla_code += [" "]
    vanilla_code += try_block()
    vanilla_code += ["  {"]
    (return_type, pointer_type) = split_type(yml_definition["ReturnType"])
    if(return_type == "Bool"):
        vanilla_code += create_generic_xloper(yml_definition)
    elif(return_type == "Double"):
        vanilla_code += create_generic_xloper(yml_definition)

    return vanilla_code


def excel_functions_base(yml_definition):
    auto_gen = []
    name = yml_definition["ExcelName"]
    line = "extern \"C\" __declspec(dllexport) \nLPXLOPER12  "
    line += name + "("
    if yml_definition["Args"]:
        for argument in yml_definition["Args"]:
            arg_type = argument["Type"]
            (rt, pt) = split_type(arg_type)
            line += XlType()
            line += " "
            line += argument["Name"] + "_input"
            line += ", "
        line = line[:-2]
    line += ")"

    auto_gen += [line]
    auto_gen += ["{ "]
    auto_gen += ["  LPXLOPER12 xloper_result = static_cast<LPXLOPER12> (calloc(1, sizeof(xloper12)));"]
    auto_gen += [" "]
    auto_gen += try_block()
    auto_gen += ["   {"]
    (return_type, pointer_type) = split_type(yml_definition["ReturnType"])
    if((return_type == "Handle") & (pointer_type == "XlDictionary")):
        auto_gen += create_cached_handle(yml_definition)

    elif((return_type == "Handle") & (pointer_type == "XlArray")):
        auto_gen += create_cached_handle(yml_definition)

    elif((return_type == "LPXLOPER") &  (pointer_type == "XlArray")):
        auto_gen += create_generic_xloper(yml_definition)

    return auto_gen

def write_file(text: str, file_name: PathLike):
    with open(file_name, "w") as output_file:
        for line in text:
            output_file.write(line + "\n")

def CreateDir(path):
    os.makedirs(path, exist_ok=True)


def parse_args(args: Iterable[str] | None = None) -> Namespace:
    """Parse incoming command-line arguments.

    Parameters
    ----------
    args : Iterable[str] | None, default=None
        Command-line arguments to parse

    Returns
    -------
    argparse.Namespace
    """
    ap = ArgumentParser(description=__doc__, formatter_class=HelpFormatter)
    ap.add_argument("input_file", type=Path, help="YAML interface input file")
    ap.add_argument(
        "--oxl-root",
        default=Path("."),
        type=Path,
        help=(
            "Directory to look for oxl/ or oxl/xl_api headers. This is similar "
            "to the --sysroot GCC option."
        )
    )
    ap.add_argument(
        "-I",
        "--include-directory",
        action="append",
        type=Path,
        default=[],
        help=(
            "Directories to search, in order, when resolving headers listed "
            "in the HeaderFiles field of the YAML input. These take precedence "
            "over --oxl-root."
        )
    )
    ap.add_argument(
        "-o",
        "--output-directory",
        default=Path("."),
        type=Path,
        help=(
            "Output directory for the generated C++ files. The .h and .cpp "
            "paths are specified via the RegFile and OutputFile fields in the "
            "YAML input file respectively."
        )
    )
    ap.add_argument(
        "-MMD",
        "--makedeps",
        action="store_true",
        help=(
            "Output a Make/CMake depfile rule for the generated .cpp file "
            "listed in the OutputFile field of the input YAML interface file."
        )
    )
    ap.add_argument(
        "-MF",
        "--makefile",
        type=Path,
        help=(
            "Specifies the alternate path to write the depfile to when used "
            "with -MMD. The default is the input file name with a .d suffixed "
            "written to the output directory."
        )
    )
    return ap.parse_args(args=args)


# TODO: document more + enable verbose output later
def main(args: Iterable[str] | None = None) -> int:
    # parse arguments
    argn = parse_args(args=args)
    # open file to load YAML document
    with open(argn.input_file, "r") as stream:
        yml = yaml.safe_load(stream)

    # create include resolver
    resolver = IncludeResolver(include_dirs=argn.include_directory + [argn.oxl_root])

    # TODO: document more
    registered_funcs = register_function(yml)
    out_regfile = argn.output_directory / yml["RegFile"]
    out_regfile.parent.mkdir(parents=True, exist_ok=True)
    write_file(registered_funcs, out_regfile)

    excel_func_cpp =  create_excel_cpp(yml, resolver=resolver)
    out_cppfile = argn.output_directory / yml["OutputFile"]
    out_cppfile.parent.mkdir(parents=True, exist_ok=True)
    write_file(excel_func_cpp, out_cppfile)

    # if -MMD specified we also create the depfile
    if argn.makedeps:
        # absolute output path
        depfile_path = (
            argn.makefile if argn.makefile
            else argn.output_directory / (argn.input_file.name + ".d")
        ).resolve()
        # write to file
        with open(depfile_path, "w") as f:
            f.write(make_depfile_content(argn.input_file, yml, resolver=resolver))
    return 0


if __name__ == "__main__":
    sys.exit(main())
