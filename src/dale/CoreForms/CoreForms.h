#ifndef DALE_COREFORMS
#define DALE_COREFORMS

#include "../Node/Node.h"
#include "../Units/Units.h"
#include "../llvm_Module.h"

namespace dale {
/*! CoreForms

    Provides for checking whether a given binding is already in use as
    a core form, and for whether a given core form may be overridden.
    init must be called before exists or existsNoOverride is called.
*/

typedef bool (*standard_core_form_t)(Units *units, Function *fn,
                                     llvm::BasicBlock *block,
                                     Node *node, bool get_address,
                                     bool prefixed_with_core,
                                     ParseResult *pr);
typedef Node *(*macro_core_form_t)(Context *ctx, Node *node);
typedef bool (*toplevel_core_form_t)(Units *units, Node *node);

namespace CoreForms {
/*! Initialise the core form functions.
 */
void init();
/*! Check whether a given binding is a core form.
 *  @param name The name of the binding.
 */
bool exists(const char *name);
/*! Check whether a given binding is a core form that may not be
 *  overridden.
 *  @param name The name of the binding.
 */
bool existsNoOverride(const char *name);
/*! Get the standard core form function pointer for the given name.
 *  @param name The name of the binding.
 *
 *  Standard core forms are those not implemented internally as
 *  macros.
 */
standard_core_form_t getStandard(const char *name);
/*! Get the macro core form function pointer for the given name.
 *  @param name The name of the binding.
 *
 *  Macro core forms are those implemented internally as macros.
 */
macro_core_form_t getMacro(const char *name);
/*! Get the top-level core form function pointer for the given name.
 *  @param name The name of the binding.
 *
 *  Top-level core forms are those that may only be called at the
 *  top level of a file.
 */
toplevel_core_form_t getTopLevel(const char *name);
}
}

#endif
