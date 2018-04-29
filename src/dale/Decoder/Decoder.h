#ifndef DALE_DECODER
#define DALE_DECODER

#include <sys/stat.h>
#include <cerrno>
#include <climits>
#include <string>
#include <vector>

#include "../Type/Type.h"
#include "../Variable/Variable.h"
#include "../Units/Units.h"

/* Functions for constructing LLVM constants from binary data. */
namespace dale {
llvm::Constant *decodeRawData(Units *units, Node *top, char *data,
                              Type *type, int *size);
}

#endif
