#include "TypeRegister.h"

#include "../Utils/Utils.h"
#include "../STLUtils/STLUtils.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace dale
{
TypeRegister::TypeRegister(void)
{
    basic_types[Type::Bool]       = new Element::Type(Type::Bool);
    basic_types[Type::Void]       = new Element::Type(Type::Void);
    basic_types[Type::VarArgs]    = new Element::Type(Type::VarArgs);
    basic_types[Type::Int]        = new Element::Type(Type::Int);
    basic_types[Type::IntPtr]     = new Element::Type(Type::IntPtr);
    basic_types[Type::Size]       = new Element::Type(Type::Size);
    basic_types[Type::PtrDiff]    = new Element::Type(Type::PtrDiff);
    basic_types[Type::UInt]       = new Element::Type(Type::UInt);
    basic_types[Type::Char]       = new Element::Type(Type::Char);
    basic_types[Type::Float]      = new Element::Type(Type::Float);
    basic_types[Type::Double]     = new Element::Type(Type::Double);
    basic_types[Type::LongDouble] = new Element::Type(Type::LongDouble);

    basic_types[Type::Int8]       = new Element::Type(Type::Int8);
    basic_types[Type::UInt8]      = new Element::Type(Type::UInt8);
    basic_types[Type::Int16]      = new Element::Type(Type::Int16);
    basic_types[Type::UInt16]     = new Element::Type(Type::UInt16);
    basic_types[Type::Int32]      = new Element::Type(Type::Int32);
    basic_types[Type::UInt32]     = new Element::Type(Type::UInt32);
    basic_types[Type::Int64]      = new Element::Type(Type::Int64);
    basic_types[Type::UInt64]     = new Element::Type(Type::UInt64);
    basic_types[Type::Int128]     = new Element::Type(Type::Int128);
    basic_types[Type::UInt128]    = new Element::Type(Type::UInt128);

    type_bool        = getBasicType(Type::Bool);
    type_void        = getBasicType(Type::Void);
    type_varargs     = getBasicType(Type::VarArgs);
    type_int         = getBasicType(Type::Int);
    type_intptr      = getBasicType(Type::IntPtr);
    type_size        = getBasicType(Type::Size);
    type_ptrdiff     = getBasicType(Type::PtrDiff);
    type_uint        = getBasicType(Type::UInt);
    type_char        = getBasicType(Type::Char);
    type_float       = getBasicType(Type::Float);
    type_double      = getBasicType(Type::Double);
    type_longdouble  = getBasicType(Type::LongDouble);

    type_int8    = getBasicType(Type::Int8);
    type_uint8   = getBasicType(Type::UInt8);
    type_int16   = getBasicType(Type::Int16);
    type_uint16  = getBasicType(Type::UInt16);
    type_int32   = getBasicType(Type::Int32);
    type_uint32  = getBasicType(Type::UInt32);
    type_int64   = getBasicType(Type::Int64);
    type_uint64  = getBasicType(Type::UInt64);
    type_int128  = getBasicType(Type::Int128);
    type_uint128 = getBasicType(Type::UInt128);

    type_pchar  = getPointerType(type_char);
    type_pvoid  = getPointerType(type_void);
}

TypeRegister::~TypeRegister(void)
{
    for (int i = 1; i < 23; i++) {
        delete basic_types[i];
    }

    stl::deleteMapElements(&pointer_types);
    stl::deleteMapElements(&const_types);
    stl::deleteMapElements(&struct_types);
    stl::deleteMapElements(&reference_types);
    stl::deleteMapElements(&retval_types);
    stl::deleteNestedMapElements(&array_types);
    stl::deleteNestedMapElements(&bitfield_types);
}

Element::Type*
TypeRegister::getBasicType(int type)
{
    return basic_types[type];
}

Element::Type*
TypeRegister::getPointerType(Element::Type *type)
{
    std::map<Element::Type*, Element::Type*>::iterator
        b = pointer_types.find(type), e = pointer_types.end();
    if (b != e) {
        return b->second;
    }

    Element::Type *pointer_type = new Element::Type(type);
    pointer_types.insert(
        std::pair<Element::Type*, Element::Type*>(type, pointer_type)
    );
    return pointer_type;
}

Element::Type*
TypeRegister::getArrayType(Element::Type *type, size_t size)
{
    std::map<Element::Type*, std::map<size_t, Element::Type*> >::iterator
        b = array_types.find(type), e = array_types.end();
    if (b != e) {
        std::map<size_t, Element::Type*>::iterator
            ab = b->second.find(size), ae = b->second.end();
        if (ab != ae) {
            return ab->second;
        } else {
            Element::Type *array_type = new Element::Type();
            array_type->is_array = 1;
            array_type->array_type = type;
            array_type->array_size = size;
            b->second.insert(
                std::pair<size_t, Element::Type*>(size, array_type)
            );
            return array_type;
        }
    }

    array_types.insert(
        std::pair<Element::Type*, std::map<size_t, Element::Type*> >(
            type, std::map<size_t, Element::Type*>()
        )
    );
    return getArrayType(type, size);
}

Element::Type*
TypeRegister::getBitfieldType(Element::Type *type, size_t size)
{
    std::map<Element::Type*, std::map<size_t, Element::Type*> >::iterator
        b = bitfield_types.find(type), e = bitfield_types.end();
    if (b != e) {
        std::map<size_t, Element::Type*>::iterator
            ab = b->second.find(size), ae = b->second.end();
        if (ab != ae) {
            return ab->second;
        } else {
            Element::Type *bitfield_type = type->makeCopy();
            bitfield_type->bitfield_size = size;
            b->second.insert(
                std::pair<size_t, Element::Type*>(size, bitfield_type)
            );
            return bitfield_type;
        }
    }

    bitfield_types.insert(
        std::pair<Element::Type*, std::map<size_t, Element::Type*> >(
            type, std::map<size_t, Element::Type*>()
        )
    );
    return getBitfieldType(type, size);
}

Element::Type*
TypeRegister::getConstType(Element::Type *type)
{
    std::map<Element::Type*, Element::Type*>::iterator
        b = const_types.find(type), e = const_types.end();
    if (b != e) {
        return b->second;
    }

    Element::Type *const_type = type->makeCopy();
    const_type->is_const = 1;
    const_types.insert(
        std::pair<Element::Type*, Element::Type*>(type, const_type)
    );
    return const_type;
}

Element::Type*
TypeRegister::getReferenceType(Element::Type *type)
{
    std::map<Element::Type*, Element::Type*>::iterator
        b = reference_types.find(type), e = reference_types.end();
    if (b != e) {
        return b->second;
    }

    Element::Type *reference_type = type->makeCopy();
    reference_type->is_reference = 1;
    reference_types.insert(
        std::pair<Element::Type*, Element::Type*>(type, reference_type)
    );
    return reference_type;
}

Element::Type*
TypeRegister::getRetvalType(Element::Type *type)
{
    std::map<Element::Type*, Element::Type*>::iterator
        b = retval_types.find(type), e = retval_types.end();
    if (b != e) {
        return b->second;
    }

    Element::Type *retval_type = type->makeCopy();
    retval_type->is_retval = 1;
    retval_types.insert(
        std::pair<Element::Type*, Element::Type*>(type, retval_type)
    );
    return retval_type;
}

Element::Type*
TypeRegister::getStructType(std::string name)
{
    std::map<std::string, Element::Type*>::iterator
        b = struct_types.find(name), e = struct_types.end();
    if (b != e) {
        return b->second;
    }

    std::vector<std::string> name_parts;
    splitString(&name, &name_parts, '.');
    Element::Type *struct_type = new Element::Type(); 
    struct_type->struct_name = new std::string(name_parts.back());
    name_parts.pop_back();

    struct_type->namespaces = new std::vector<std::string>(name_parts);

    struct_types.insert(
        std::pair<std::string, Element::Type*>(name, struct_type)
    );
    return struct_type;
}

Element::Type*
TypeRegister::getType(Element::Type *type)
{
    Element::Type *final = NULL;

    if (type->is_const) {
        type->is_const = 0;
        final = getConstType(getType(type));
        type->is_const = 1;
    } else if (type->is_reference) {
        type->is_reference = 0;
        final = getReferenceType(getType(type));
        type->is_reference = 1;
    } else if (type->is_array) {
        final = getArrayType(getType(type->array_type), type->array_size);
    } else if (type->points_to) {
        final = getPointerType(getType(type->points_to));
    } else if (type->struct_name) {
        std::string name;
        for (std::vector<std::string>::iterator
                b = type->namespaces->begin(),
                e = type->namespaces->end();
                b != e;
                ++b) {
            name.append((*b));
            name.append(".");
        }
        name.append(*(type->struct_name));
        final = getStructType(name);
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

void
TypeRegister::dump(void)
{
    fprintf(stderr, "Pointer type count: %lu\n", pointer_types.size());
    fprintf(stderr, "Const type count:   %lu\n", const_types.size());
    fprintf(stderr, "Array type count:   %lu\n", array_types.size());
    fprintf(stderr, "BF type count:      %lu\n", bitfield_types.size());
    fprintf(stderr, "Struct type count:  %lu\n", struct_types.size());
}
}
