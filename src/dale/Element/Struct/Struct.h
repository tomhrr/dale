#ifndef DALE_ELEMENT_STRUCT
#define DALE_ELEMENT_STRUCT

#include "../Type/Type.h"
#include "../../Linkage/Linkage.h"

#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>

#include "llvm/Module.h"
#include "llvm/Function.h"
#include "llvm/PassManager.h"
#include "llvm/CallingConv.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"

namespace dale
{
namespace Element
{
class Struct
{
public:
    llvm::StructType *type;
    int is_opaque;
    std::string *internal_name;
    std::vector<Element::Type *> *element_types;
    std::map<std::string, int> *names_to_numbers;
    std::string once_tag;
    int linkage;
    int must_init;
    bool serialise;

    Struct();
    ~Struct();

    int addElement(const char *name, Element::Type *type);
    Element::Type* nameToType(const char *name);
    Element::Type* indexToType(int index);
    int nameToIndex(const char *name);
    const char *indexToName(int index);
};
}
}

#endif
