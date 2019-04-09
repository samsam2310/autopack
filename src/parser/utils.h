#include <map>
#include <string>

#ifndef TYPE_H
#define TYPE_H

class ExportType {
public:
    const static std::map<std::string, std::string> c_type_map;
    const static std::map<std::string, std::string> js_type_map;
    static std::map<std::string, std::string> create_c_map() {
        std::map<std::string, std::string> c_map = {
            {"short", "i"},
            {"short int", "i"},
            {"signed short", "i"},
            {"signed short int", "i"},
            {"unsigned short", "u"},
            {"unsigned short int", "u"},
            {"int", "i"},
            {"signed", "i"},
            {"signed int", "i"},
            {"unsigned", "u"},
            {"unsigned int", "u"},
            {"long", "i"},
            {"long int", "i"},
            {"signed long", "i"},
            {"signed long int", "i"},
            {"unsigned long", "u"},
            {"unsigned long int", "u"},
            {"long long", "i"},
            {"long long int", "i"},
            {"signed long long", "i"},
            {"signed long long int", "i"},
            {"unsigned long long", "u"},
            {"unsigned long long int", "u"},
            {"float", "f"},
            {"double", "f"},
            {"long double", "f"},
            {"bool", "b"},
            {"signed char", "i"},
            {"unsigned char", "i"},
            {"char", "i"}
        };
        return c_map;
    }
    static std::map<std::string, std::string> create_js_map() {
        std::map<std::string, std::string> js_map = {
            {"short", "CShort"},
            {"short int", "CShort"},
            {"signed short", "CShort"},
            {"signed short int", "CShort"},
            {"unsigned short", "CUnsignedShort"},
            {"unsigned short int", "CUnsignedShort"},
            {"int", "CLong"},
            {"signed", "CLong"},
            {"signed int", "CLong"},
            {"unsigned", "CUnsignedLong"},
            {"unsigned int", "CUnsignedLong"},
            {"long", "CLong"},
            {"long int", "CLong"},
            {"signed long", "CLong"},
            {"signed long int", "CLong"},
            {"unsigned long", "CUnsignedLong"},
            {"unsigned long int", "CUnsignedLong"},
            {"long long", "CLongLong"},
            {"long long int", "CLongLong"},
            {"signed long long", "CLongLong"},
            {"signed long long int", "CLongLong"},
            {"unsigned long long", "CUnsignedLongLong"},
            {"unsigned long long int", "CUnsignedLongLong"},
            {"float", "CFloat"},
            {"double", "CDouble"},
            {"long double", "CDouble"},
            {"bool", "CBool"},
            {"signed char", "CChar"},
            {"unsigned char", "CChar"},
            {"char", "CChar"} 
        };
        return js_map;
    }
};
const std::map<std::string, std::string> ExportType::c_type_map =
    ExportType::create_c_map();
const std::map<std::string, std::string> ExportType::js_type_map =
    ExportType::create_js_map();

class TemplateSet {
public:
    const static std::string js_pure_function_entity;
};

const std::string TemplateSet::js_pure_function_entity = R"(
    C.%s = (...args) => {
      const ident = getCFuncIdent(wasmModule, '_%s', args);
      const hint = hints[ident];
      return callCFunction(
        wasmModule,
        hint[0],
        ident,
        hint[1],
        hint[2],
        args,
        null
      );
    };
)";

#endif
