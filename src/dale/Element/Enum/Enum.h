#ifndef DALE_ELEMENT_ENUM
#define DALE_ELEMENT_ENUM

#include "../Type/Type.h"
#include "../../Linkage/Linkage.h"

#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>

#include "../../llvm_Module.h"
#include "../../llvm_Function.h"
#include "llvm/PassManager.h"
#include "../../llvm_CallingConv.h"
#include "../../llvm_AnalysisVerifier.h"
#include "../../llvm_AssemblyPrintModulePass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "../../llvm_IRBuilder.h"

#define ENUM_NOTFOUND -1000000

namespace dale
{
namespace Element
{
class Enum
{
public:
    llvm::Type *type;
    int last_index;
    std::map<std::string, int64_t> *names_to_numbers;
    std::string once_tag;
    int linkage;
    bool serialise;

    Enum();
    ~Enum();

    int addElement(const char *name);
    int addElement(const char *name, int64_t number);
    int64_t nameToNumber(const char *name);
};
}
}

#endif
