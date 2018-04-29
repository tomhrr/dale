#include "TypeRegister.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../STL/STL.h"
#include "../Utils/Utils.h"

namespace dale {
TypeRegister::TypeRegister() {
    basic_types[BaseType::Bool] = new Type(BaseType::Bool);
    basic_types[BaseType::Void] = new Type(BaseType::Void);
    basic_types[BaseType::VarArgs] = new Type(BaseType::VarArgs);
    basic_types[BaseType::Int] = new Type(BaseType::Int);
    basic_types[BaseType::IntPtr] = new Type(BaseType::IntPtr);
    basic_types[BaseType::Size] = new Type(BaseType::Size);
    basic_types[BaseType::PtrDiff] = new Type(BaseType::PtrDiff);
    basic_types[BaseType::UInt] = new Type(BaseType::UInt);
    basic_types[BaseType::Char] = new Type(BaseType::Char);
    basic_types[BaseType::Float] = new Type(BaseType::Float);
    basic_types[BaseType::Double] = new Type(BaseType::Double);
    basic_types[BaseType::LongDouble] = new Type(BaseType::LongDouble);

    basic_types[BaseType::Int8] = new Type(BaseType::Int8);
    basic_types[BaseType::UInt8] = new Type(BaseType::UInt8);
    basic_types[BaseType::Int16] = new Type(BaseType::Int16);
    basic_types[BaseType::UInt16] = new Type(BaseType::UInt16);
    basic_types[BaseType::Int32] = new Type(BaseType::Int32);
    basic_types[BaseType::UInt32] = new Type(BaseType::UInt32);
    basic_types[BaseType::Int64] = new Type(BaseType::Int64);
    basic_types[BaseType::UInt64] = new Type(BaseType::UInt64);
    basic_types[BaseType::Int128] = new Type(BaseType::Int128);
    basic_types[BaseType::UInt128] = new Type(BaseType::UInt128);

    type_bool = getBasicType(BaseType::Bool);
    type_void = getBasicType(BaseType::Void);
    type_varargs = getBasicType(BaseType::VarArgs);
    type_int = getBasicType(BaseType::Int);
    type_intptr = getBasicType(BaseType::IntPtr);
    type_size = getBasicType(BaseType::Size);
    type_ptrdiff = getBasicType(BaseType::PtrDiff);
    type_uint = getBasicType(BaseType::UInt);
    type_char = getBasicType(BaseType::Char);
    type_float = getBasicType(BaseType::Float);
    type_double = getBasicType(BaseType::Double);
    type_longdouble = getBasicType(BaseType::LongDouble);

    type_int8 = getBasicType(BaseType::Int8);
    type_uint8 = getBasicType(BaseType::UInt8);
    type_int16 = getBasicType(BaseType::Int16);
    type_uint16 = getBasicType(BaseType::UInt16);
    type_int32 = getBasicType(BaseType::Int32);
    type_uint32 = getBasicType(BaseType::UInt32);
    type_int64 = getBasicType(BaseType::Int64);
    type_uint64 = getBasicType(BaseType::UInt64);
    type_int128 = getBasicType(BaseType::Int128);
    type_uint128 = getBasicType(BaseType::UInt128);

    type_pchar = getPointerType(type_char);
    type_pvoid = getPointerType(type_void);

    type_dnode = getStructType("DNode");
    type_pdnode = getPointerType(type_dnode);
}

TypeRegister::~TypeRegister() {
    for (int i = 1; i < 23; i++) {
        delete basic_types[i];
    }

    STL::deleteMapElements(&pointer_types);
    STL::deleteMapElements(&const_types);
    STL::deleteMapElements(&struct_types);
    STL::deleteMapElements(&reference_types);
    STL::deleteMapElements(&rvalue_reference_types);
    STL::deleteMapElements(&retval_types);
    STL::deleteNestedMapElements(&array_types);
    STL::deleteNestedMapElements(&bitfield_types);
}

Type *TypeRegister::getBasicType(int type) { return basic_types[type]; }

Type *TypeRegister::getPointerType(Type *type) {
    std::map<Type *, Type *>::iterator b = pointer_types.find(type),
                                       e = pointer_types.end();
    if (b != e) {
        return b->second;
    }

    Type *pointer_type = new Type(type);
    pointer_types.insert(std::pair<Type *, Type *>(type, pointer_type));
    return pointer_type;
}

Type *TypeRegister::getArrayType(Type *type, size_t size) {
    std::map<Type *, std::map<size_t, Type *> >::iterator
        b = array_types.find(type),
        e = array_types.end();
    if (b != e) {
        std::map<size_t, Type *>::iterator ab = b->second.find(size),
                                           ae = b->second.end();
        if (ab != ae) {
            return ab->second;
        } else {
            Type *array_type = new Type();
            array_type->is_array = true;
            array_type->array_type = type;
            array_type->array_size = size;
            b->second.insert(
                std::pair<size_t, Type *>(size, array_type));
            return array_type;
        }
    }

    array_types.insert(std::pair<Type *, std::map<size_t, Type *> >(
        type, std::map<size_t, Type *>()));
    return getArrayType(type, size);
}

Type *TypeRegister::getBitfieldType(Type *type, size_t size) {
    std::map<Type *, std::map<size_t, Type *> >::iterator
        b = bitfield_types.find(type),
        e = bitfield_types.end();
    if (b != e) {
        std::map<size_t, Type *>::iterator ab = b->second.find(size),
                                           ae = b->second.end();
        if (ab != ae) {
            return ab->second;
        } else {
            Type *bitfield_type = type->makeCopy();
            bitfield_type->bitfield_size = size;
            b->second.insert(
                std::pair<size_t, Type *>(size, bitfield_type));
            return bitfield_type;
        }
    }

    bitfield_types.insert(std::pair<Type *, std::map<size_t, Type *> >(
        type, std::map<size_t, Type *>()));
    return getBitfieldType(type, size);
}

Type *TypeRegister::getConstType(Type *type) {
    std::map<Type *, Type *>::iterator b = const_types.find(type),
                                       e = const_types.end();
    if (b != e) {
        return b->second;
    }

    Type *const_type = type->makeCopy();
    const_type->is_const = true;
    const_types.insert(std::pair<Type *, Type *>(type, const_type));
    return const_type;
}

Type *TypeRegister::getReferenceType(Type *type) {
    std::map<Type *, Type *>::iterator b = reference_types.find(type),
                                       e = reference_types.end();
    if (b != e) {
        return b->second;
    }

    Type *reference_type = type->makeCopy();
    reference_type->is_reference = true;
    reference_types.insert(
        std::pair<Type *, Type *>(type, reference_type));
    return reference_type;
}

Type *TypeRegister::getRvalueReferenceType(Type *type) {
    std::map<Type *, Type *>::iterator b = rvalue_reference_types.find(
                                           type),
                                       e = rvalue_reference_types.end();
    if (b != e) {
        return b->second;
    }

    Type *rvalue_reference_type = type->makeCopy();
    rvalue_reference_type->is_rvalue_reference = true;
    rvalue_reference_types.insert(
        std::pair<Type *, Type *>(type, rvalue_reference_type));
    return rvalue_reference_type;
}

Type *TypeRegister::getRetvalType(Type *type) {
    std::map<Type *, Type *>::iterator b = retval_types.find(type),
                                       e = retval_types.end();
    if (b != e) {
        return b->second;
    }

    Type *retval_type = type->makeCopy();
    retval_type->is_retval = true;
    retval_types.insert(std::pair<Type *, Type *>(type, retval_type));
    return retval_type;
}

Type *TypeRegister::getStructType(const char *name) {
    std::map<std::string, Type *>::iterator b = struct_types.find(name),
                                            e = struct_types.end();
    if (b != e) {
        return b->second;
    }

    std::vector<std::string> name_parts;
    std::string ss(name);
    splitString(&ss, &name_parts, '.');
    Type *struct_type = new Type();
    struct_type->struct_name = name_parts.back();
    name_parts.pop_back();

    struct_type->namespaces = name_parts;

    struct_types.insert(
        std::pair<std::string, Type *>(name, struct_type));
    return struct_type;
}

Type *TypeRegister::getType(Type *type) {
    Type *final = NULL;

    if (type->is_const) {
        type->is_const = false;
        final = getConstType(getType(type));
        type->is_const = true;
    } else if (type->is_reference) {
        type->is_reference = false;
        final = getReferenceType(getType(type));
        type->is_reference = true;
    } else if (type->is_rvalue_reference) {
        type->is_rvalue_reference = false;
        final = getRvalueReferenceType(getType(type));
        type->is_rvalue_reference = true;
    } else if (type->is_retval) {
        type->is_retval = false;
        final = getRetvalType(getType(type));
        type->is_retval = true;
    } else if (type->is_array) {
        final =
            getArrayType(getType(type->array_type), type->array_size);
    } else if (type->points_to) {
        final = getPointerType(getType(type->points_to));
    } else if (type->struct_name.size()) {
        std::string name;
        for (std::vector<std::string>::iterator
                 b = type->namespaces.begin(),
                 e = type->namespaces.end();
             b != e; ++b) {
            name.append((*b));
            name.append(".");
        }
        name.append(type->struct_name);
        final = getStructType(name.c_str());
    } else if (type->bitfield_size) {
        size_t bitfield_size = type->bitfield_size;
        type->bitfield_size = 0;
        final = getBitfieldType(getType(type), bitfield_size);
        type->bitfield_size = bitfield_size;
    } else if (type->base_type) {
        final = getBasicType(type->base_type);
    }

    return (final) ? final : type->makeCopy();
}

void TypeRegister::print() {
    fprintf(stderr, "Pointer type count: %zu\n", pointer_types.size());
    fprintf(stderr, "Const type count:   %zu\n", const_types.size());
    fprintf(stderr, "Array type count:   %zu\n", array_types.size());
    fprintf(stderr, "BF type count:      %zu\n", bitfield_types.size());
    fprintf(stderr, "Struct type count:  %zu\n", struct_types.size());
}
}
