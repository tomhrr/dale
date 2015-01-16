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
    basic_types[BaseType::Bool]       = new Element::Type(BaseType::Bool);
    basic_types[BaseType::Void]       = new Element::Type(BaseType::Void);
    basic_types[BaseType::VarArgs]    = new Element::Type(BaseType::VarArgs);
    basic_types[BaseType::Int]        = new Element::Type(BaseType::Int);
    basic_types[BaseType::IntPtr]     = new Element::Type(BaseType::IntPtr);
    basic_types[BaseType::Size]       = new Element::Type(BaseType::Size);
    basic_types[BaseType::PtrDiff]    = new Element::Type(BaseType::PtrDiff);
    basic_types[BaseType::UInt]       = new Element::Type(BaseType::UInt);
    basic_types[BaseType::Char]       = new Element::Type(BaseType::Char);
    basic_types[BaseType::Float]      = new Element::Type(BaseType::Float);
    basic_types[BaseType::Double]     = new Element::Type(BaseType::Double);
    basic_types[BaseType::LongDouble] = new Element::Type(BaseType::LongDouble);

    basic_types[BaseType::Int8]       = new Element::Type(BaseType::Int8);
    basic_types[BaseType::UInt8]      = new Element::Type(BaseType::UInt8);
    basic_types[BaseType::Int16]      = new Element::Type(BaseType::Int16);
    basic_types[BaseType::UInt16]     = new Element::Type(BaseType::UInt16);
    basic_types[BaseType::Int32]      = new Element::Type(BaseType::Int32);
    basic_types[BaseType::UInt32]     = new Element::Type(BaseType::UInt32);
    basic_types[BaseType::Int64]      = new Element::Type(BaseType::Int64);
    basic_types[BaseType::UInt64]     = new Element::Type(BaseType::UInt64);
    basic_types[BaseType::Int128]     = new Element::Type(BaseType::Int128);
    basic_types[BaseType::UInt128]    = new Element::Type(BaseType::UInt128);

    type_bool        = getBasicType(BaseType::Bool);
    type_void        = getBasicType(BaseType::Void);
    type_varargs     = getBasicType(BaseType::VarArgs);
    type_int         = getBasicType(BaseType::Int);
    type_intptr      = getBasicType(BaseType::IntPtr);
    type_size        = getBasicType(BaseType::Size);
    type_ptrdiff     = getBasicType(BaseType::PtrDiff);
    type_uint        = getBasicType(BaseType::UInt);
    type_char        = getBasicType(BaseType::Char);
    type_float       = getBasicType(BaseType::Float);
    type_double      = getBasicType(BaseType::Double);
    type_longdouble  = getBasicType(BaseType::LongDouble);

    type_int8    = getBasicType(BaseType::Int8);
    type_uint8   = getBasicType(BaseType::UInt8);
    type_int16   = getBasicType(BaseType::Int16);
    type_uint16  = getBasicType(BaseType::UInt16);
    type_int32   = getBasicType(BaseType::Int32);
    type_uint32  = getBasicType(BaseType::UInt32);
    type_int64   = getBasicType(BaseType::Int64);
    type_uint64  = getBasicType(BaseType::UInt64);
    type_int128  = getBasicType(BaseType::Int128);
    type_uint128 = getBasicType(BaseType::UInt128);

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
TypeRegister::print(void)
{
    fprintf(stderr, "Pointer type count: %lu\n", pointer_types.size());
    fprintf(stderr, "Const type count:   %lu\n", const_types.size());
    fprintf(stderr, "Array type count:   %lu\n", array_types.size());
    fprintf(stderr, "BF type count:      %lu\n", bitfield_types.size());
    fprintf(stderr, "Struct type count:  %lu\n", struct_types.size());
}
}
