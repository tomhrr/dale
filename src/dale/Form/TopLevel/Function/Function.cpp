#include "Function.h"
#include "Config.h"
#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../Function/Function.h"

namespace dale
{
bool
FormTopLevelFunctionParse(Generator *gen, Node *node)
{
    return FormFunctionParse(gen, node, NULL, NULL,
                                       Linkage::Null, 0);
}
}
