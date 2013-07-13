#include "TypeRegister.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace dale
{
TypeRegister::TypeRegister()
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
}
