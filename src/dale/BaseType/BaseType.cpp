#include "BaseType.h"

namespace dale
{
int
getSignedIntegerType(int size)
{
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

int
getUnsignedIntegerType(int size)
{
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

const char *baseTypeToString(int base_type)
{
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
}
