#include "BaseType.h"

#include <cstring>

namespace dale {
int getSignedIntegerType(int size) {
    int type;
    switch (size) {
        case 8:     type = BaseType::Int8;      break;
        case 16:    type = BaseType::Int16;     break;
        case 32:    type = BaseType::Int32;     break;
        case 64:    type = BaseType::Int64;     break;
        case 128:   type = BaseType::Int128;    break;
        default:    type = BaseType::Int;
    }
    return type;
}

int getUnsignedIntegerType(int size) {
    int type;
    switch (size) {
        case 8:     type = BaseType::UInt8;      break;
        case 16:    type = BaseType::UInt16;     break;
        case 32:    type = BaseType::UInt32;     break;
        case 64:    type = BaseType::UInt64;     break;
        case 128:   type = BaseType::UInt128;    break;
        default:    type = BaseType::UInt;
    }
    return type;
}

const char * baseTypeToString(int base_type) {
    const char *ret;
    switch (base_type) {
        case BaseType::Void:       ret = "void";        break;
        case BaseType::Bool:       ret = "bool";        break;
        case BaseType::Char:       ret = "char";        break;
        case BaseType::VarArgs:    ret = "...";         break;
        case BaseType::Int:        ret = "int";         break;
        case BaseType::UInt:       ret = "uint";        break;
        case BaseType::IntPtr:     ret = "intptr";      break;
        case BaseType::Size:       ret = "size" ;       break;
        case BaseType::PtrDiff:    ret = "ptrdiff";     break;
        case BaseType::Int8:       ret = "int8";        break;
        case BaseType::UInt8:      ret = "uint8";       break;
        case BaseType::Int16:      ret = "int16";       break;
        case BaseType::UInt16:     ret = "uint16";      break;
        case BaseType::Int32:      ret = "int32";       break;
        case BaseType::UInt32:     ret = "uint32";      break;
        case BaseType::Int64:      ret = "int64";       break;
        case BaseType::UInt64:     ret = "uint64";      break;
        case BaseType::Int128:     ret = "int128";      break;
        case BaseType::UInt128:    ret = "uint128";     break;
        case BaseType::Float:      ret = "float";       break;
        case BaseType::Double:     ret = "double";      break;
        case BaseType::LongDouble: ret = "long-double"; break;
        default:                   ret = "[unknown]";
    }
    return ret;
}

int stringToBaseType(const char *str) {
    return
          (!strcmp(str, "int"))         ? BaseType::Int
        : (!strcmp(str, "void"))        ? BaseType::Void
        : (!strcmp(str, "char"))        ? BaseType::Char
        : (!strcmp(str, "bool"))        ? BaseType::Bool
        : (!strcmp(str, "uint"))        ? BaseType::UInt
        : (!strcmp(str, "int8"))        ? BaseType::Int8
        : (!strcmp(str, "uint8"))       ? BaseType::UInt8
        : (!strcmp(str, "int16"))       ? BaseType::Int16
        : (!strcmp(str, "uint16"))      ? BaseType::UInt16
        : (!strcmp(str, "int32"))       ? BaseType::Int32
        : (!strcmp(str, "uint32"))      ? BaseType::UInt32
        : (!strcmp(str, "int64"))       ? BaseType::Int64
        : (!strcmp(str, "uint64"))      ? BaseType::UInt64
        : (!strcmp(str, "int128"))      ? BaseType::Int128
        : (!strcmp(str, "uint128"))     ? BaseType::UInt128
        : (!strcmp(str, "intptr"))      ? BaseType::IntPtr
        : (!strcmp(str, "size"))        ? BaseType::Size
        : (!strcmp(str, "ptrdiff"))     ? BaseType::PtrDiff
        : (!strcmp(str, "float"))       ? BaseType::Float
        : (!strcmp(str, "double"))      ? BaseType::Double
        : (!strcmp(str, "long-double")) ? BaseType::LongDouble
                                        : -1;
}

char baseTypeToSymbolChar(int base_type) {
    char c;
    switch (base_type) {
        case BaseType::Void:       c = 'v';             break;
        case BaseType::Bool:       c = 'b';             break;
        case BaseType::Char:       c = 'c';             break;
        case BaseType::Int:        c = 'i';             break;
        case BaseType::VarArgs:    c = 'z';             break;
        case BaseType::UInt:       c = 'j';             break;
        case BaseType::IntPtr:     c = 'l';             break;
        case BaseType::Int8:       c = 'y';             break;
        case BaseType::UInt8:      c = 'm';             break;
        case BaseType::Int16:      c = 'n';             break;
        case BaseType::UInt16:     c = 'o';             break;
        case BaseType::Int32:      c = 'p';             break;
        case BaseType::UInt32:     c = 'q';             break;
        case BaseType::Int64:      c = 'r';             break;
        case BaseType::UInt64:     c = 's';             break;
        case BaseType::Int128:     c = 't';             break;
        case BaseType::UInt128:    c = 'u';             break;
        case BaseType::Float:      c = 'f';             break;
        case BaseType::Double:     c = 'd';             break;
        case BaseType::LongDouble: c = 'a';             break;
        case BaseType::Size:       c = 'w';             break;
        case BaseType::PtrDiff:    c = 'x';             break;
        default:                   c = '?';
    }
    return c;
}

bool baseTypeIsIntegerType(int base_type) {
    return (   base_type == BaseType::Int
            || base_type == BaseType::Char
            || base_type == BaseType::UInt
            || base_type == BaseType::IntPtr
            || base_type == BaseType::Size
            || base_type == BaseType::PtrDiff
            || base_type == BaseType::Int8
            || base_type == BaseType::UInt8
            || base_type == BaseType::Int16
            || base_type == BaseType::UInt16
            || base_type == BaseType::Int32
            || base_type == BaseType::UInt32
            || base_type == BaseType::Int64
            || base_type == BaseType::UInt64
            || base_type == BaseType::Int128
            || base_type == BaseType::UInt128);
}

bool baseTypeIsSignedIntegerType(int base_type) {
    return (   base_type == BaseType::Int
            || base_type == BaseType::Char
            || base_type == BaseType::Int8
            || base_type == BaseType::Int16
            || base_type == BaseType::Int32
            || base_type == BaseType::Int64
            || base_type == BaseType::Int128);
}

int integerTypeToSize(int base_type) {
    int size;
    switch (base_type) {
        case BaseType::Char:    size = 8;       break;
        case BaseType::Int:
        case BaseType::UInt:    size = 1;       break;
        case BaseType::IntPtr:  size = 2;       break;
        case BaseType::Size:    size = 3;       break;
        case BaseType::PtrDiff: size = 4;       break;
        case BaseType::Int8:
        case BaseType::UInt8:   size = 8;       break;
        case BaseType::Int16:
        case BaseType::UInt16:  size = 16;      break;
        case BaseType::Int32:
        case BaseType::UInt32:  size = 32;      break;
        case BaseType::Int64:
        case BaseType::UInt64:  size = 64;      break;
        case BaseType::Int128:
        case BaseType::UInt128: size = 128;     break;
        default:                size = 0;
    }
    return size;
}

bool baseTypeIsFloatingPointType(int base_type) {
    return (   base_type == BaseType::Float
            || base_type == BaseType::Double
            || base_type == BaseType::LongDouble);
}

int floatingPointTypeToRelativeSize(int base_type) {
    int size;
    switch (base_type) {
        case BaseType::Float:       size = 1;       break;
        case BaseType::Double:      size = 2;       break;
        case BaseType::LongDouble:  size = 3;       break;
        default:                    size = 0;
    }
    return size;
}
}
