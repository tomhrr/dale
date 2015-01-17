#ifndef DALE_ELEMENT_STRUCT
#define DALE_ELEMENT_STRUCT

#include "../Type/Type.h"
#include "../Linkage/Linkage.h"

#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>

#include "../llvm_Module.h"
#include "../llvm_Function.h"
#include "llvm/PassManager.h"
#include "../llvm_CallingConv.h"
#include "../llvm_AnalysisVerifier.h"
#include "../llvm_AssemblyPrintModulePass.h"
#include "../llvm_IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"

namespace dale
{
class Struct
{
public:
    llvm::StructType *type;
    int is_opaque;
    std::string internal_name;
    std::vector<Type *> element_types;
    std::map<std::string, int> names_to_numbers;
    std::string once_tag;
    int linkage;
    int must_init;
    bool serialise;

    Struct();
    ~Struct();

    int addElement(const char *name, Type *type);
    Type* nameToType(const char *name);
    Type* indexToType(int index);
    int nameToIndex(const char *name);
    const char *indexToName(int index);
};
}

#endif
