#ifndef DALE_LINKAGE
#define DALE_LINKAGE

namespace dale
{
namespace Linkage
{
enum
{
    Null,
    Auto,
    Intern,
    Extern,
    Extern_C,
    Extern_Weak
};

bool isExtern(int linkage);
bool isExternAll(int linkage);
}

namespace StructLinkage
{
enum
{
    Null,
    Intern,
    Extern,
    Opaque
};
bool isExtern(int linkage);
}

namespace EnumLinkage
{
enum
{
    Null,
    Intern,
    Extern
};
bool isExtern(int linkage);
}
}

#endif
