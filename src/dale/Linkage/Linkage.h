#ifndef DALE_LINKAGE
#define DALE_LINKAGE

namespace dale {
/*! Linkage

    Contains the variable and function linkage definitions.
*/
namespace Linkage {
enum { Null, Auto, Intern, Extern, Extern_C, Extern_Weak };

/*! Check whether the linkage is an extern linkage.
 *  @param linkage The linkage.
 */
bool isExtern(int linkage);
/*! Check whether the linkage is an extern linkage (includes weak).
 *  @param linkage The linkage.
 */
bool isExternAll(int linkage);
}

/*! StructLinkage

    Contains the struct linkage definitions.
*/
namespace StructLinkage {
enum { Null, Intern, Extern, Opaque };
/*! Check whether the linkage is an extern linkage.
 *  @param linkage The linkage.
 */
bool isExtern(int linkage);
}

/*! EnumLinkage

    Contains the enum linkage definitions.
*/
namespace EnumLinkage {
enum { Null, Intern, Extern };
/*! Check whether the linkage is an extern linkage.
 *  @param linkage The linkage.
 */
bool isExtern(int linkage);
}
}

#endif
