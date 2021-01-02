#ifndef DALE_DECODER
#define DALE_DECODER

#include <sys/stat.h>
#include <cerrno>
#include <climits>
#include <string>
#include <vector>

#include "../Type/Type.h"
#include "../Units/Units.h"
#include "../Variable/Variable.h"

namespace dale {
/* Construct an LLVM constant from binary data.
 * @param units The units context.
 * @param top The node, for error references.
 * @param data The binary data.
 * @param type The type of the data.
 * @param size If the value is a string, the size will be stored here.
 * @param prev_mod The module into which new variables should be written.
 *
 * To handle binary data that includes pointers, the retrieval log
 * must be enabled during parsing of the relevant nodes.  See
 * enableRetrievalLog and disableRetrievalLog in Context.
 */
llvm::Constant *decodeRawData(Units *units, Node *top, char *data,
                              Type *type, int *size,
                              llvm::Module *prev_mod);
}

#endif
