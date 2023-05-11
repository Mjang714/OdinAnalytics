import sys
import os
import yaml


#this function creates the the header function for the registration
def header_reg_func(num_func, prefix_reg):
    print("Generating header file for " + prefix_reg)
    xl_reg_hdr = []
    xl_reg_hdr.append("// This is the registration funcition header that excel will use expose the function outwards.")
    xl_reg_hdr.append("// This auto generated header registration file that leverages the yaml file.")
    xl_reg_hdr.append("  ")
    xl_reg_hdr.append("#include <format>")
    xl_reg_hdr.append("#include <stdlib.h>")  
    xl_reg_hdr.append("#define NOMINMAX")
    xl_reg_hdr.append("#include <iostream>")
    xl_reg_hdr.append("#include <Windows.h>")
    xl_reg_hdr.append("#include <string>")
    xl_reg_hdr.append("#include \"xlcall.h\"")
    xl_reg_hdr.append("#include \"framewrk.h\"")
   
    xl_reg_hdr.append("#define " + prefix_reg + "_num_reg_functions_rows "+ str(num_func))
    xl_reg_hdr.append(" ")
    xl_reg_hdr.append("static LPWSTR "+ prefix_reg + "_ngFuncs[" + prefix_reg + "_num_reg_functions_rows][12] = {")
    
    print("Finished registration header creation for " + prefix_reg)

    return xl_reg_hdr

def regsiter_function(yml_defintion):
    rfs = header_reg_func(len(yml_defintion["Functions"]), yml_defintion["RegPreFix"])

    for func_desc in yml_defintion["Functions"]:
        args = func_desc["Args"]
        line ="{ L\""
        line += func_desc["ExcelName"]
        line += "\", L\""
        line += "Q"
        if args:
            for arg in args:
                line+="Q"
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

def create_excel_cpp(yml_defintiion):
    auto_gen = excel_function_header(yml_defintiion)
    for func_desc in yml_defintiion["Functions"]:
        func_type = func_desc["Type"]
        if(func_type == "Excel"):
            auto_gen += excel_functions_base(func_desc)
        elif(func_type == "Vanilla"):
            auto_gen += vanilla_functions_base(func_desc)
        auto_gen += ["       return xloper_result;"]
        # create another else here for return type that is Array for the line above  
        auto_gen += ["   }"]
        auto_gen += [" "]
        auto_gen += catch_block()
        auto_gen += ["}"]

    return auto_gen        

def excel_function_header(yml_defintion):
    header = []
    header.append("// This is the registration funcition cpp that excel will use expose the function outwards.")
    header.append("// This auto generated cpp registration file that leverages the yaml file.")
    header.append("  ")
    header.append("#include <format>")
    header.append("#include <stdlib.h>")  
    header.append("#define NOMINMAX")
    header.append("#include <iostream>")
    header.append("#include <Windows.h>")
    header.append("#include <string>")
    header.append("#include \"xlcall.h\"")
    header.append("#include \"framewrk.h\"")
    header.append("#include \"oxl/xl_api/xloper_converter.h\"")
    header.append("#include \"oxl/xl_api/cache_xl_obj.h\"")
    if(yml_defintion["HeaderFiles"]):
        for header_file in yml_defintion["HeaderFiles"]:
            header.append("#include \"oxl/" + header_file["FileName"]+"\"")
    header.append(" ")
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
    auto_gen_code += ["       oxl::xl_api::XlCacheObj cached_obj = oxl::xl_api::XlCacheObj(cache_obj, base_name);"]
    auto_gen_code += ["       oxl::xl_api::XLoperObj::ConvertToLPXloper(cached_obj.CacheName(), xloper_result);"]
    return auto_gen_code

def create_generic_xloper(yml_definition):
    auto_gen_code = []
    line = "       auto intermediate_result = oxl::" + yml_definition["OxlName"]+ "("
    argline = " "
    if (yml_definition["Args"]) :
        for arg in yml_definition["Args"]:
            argline += arg["Name"] + "_input,"
        line += argline[:-1]
        line += " );"
    auto_gen_code += [line]
    auto_gen_code += ["       oxl::xl_api::XLoperObj::ConvertToLPXloper(intermediate_result, xloper_result);"]
    return auto_gen_code   

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

def write_file(text, file_name):
    with open(file_name, "w") as output_file:
        for line in text:
            output_file.write(line + "\n")
   
def CreateDir(path):
     os.makedirs(path, exist_ok=True)

def main():

    print("starting the autogen")

    arg_vector = sys.argv

    xl_yaml_file = arg_vector[1]
    output_path = arg_vector[2]

    print("Opening yaml config file: " + xl_yaml_file)
    print("Outputing files to : " + output_path)

    with open(xl_yaml_file, "r") as stream:
        yaml_file_stream = yaml.safe_load(stream)

    CreateDir(output_path)
    registered_funcs = regsiter_function(yaml_file_stream)
    write_file(registered_funcs, output_path + "\\" + yaml_file_stream["RegFile"])

    excel_func_cpp =  create_excel_cpp(yaml_file_stream)
    write_file(excel_func_cpp, output_path + "\\" + yaml_file_stream["OutputFile"])

main()
