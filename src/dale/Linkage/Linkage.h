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
}

namespace EnumLinkage
{
enum
{
    Null,
    Intern,
    Extern
};
}
}

#endif
