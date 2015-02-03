#include "Linkage.h"

namespace dale
{
bool
Linkage::isExtern(int linkage)
{
    return ((linkage == Linkage::Extern)
         || (linkage == Linkage::Extern_C));
}

bool
Linkage::isExternAll(int linkage)
{
    return ((linkage == Linkage::Extern)
         || (linkage == Linkage::Extern_C)
         || (linkage == Linkage::Extern_Weak));
}

bool
StructLinkage::isExtern(int linkage)
{
    return ((linkage == StructLinkage::Extern)
         || (linkage == StructLinkage::Opaque));
}

bool
EnumLinkage::isExtern(int linkage)
{
    return (linkage == EnumLinkage::Extern);
}
}
