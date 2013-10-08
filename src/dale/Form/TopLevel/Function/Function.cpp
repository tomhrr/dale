#include "Function.h"
#include "Config.h"
#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../Function/Function.h"

namespace dale
{
namespace Form
{
namespace TopLevel
{
namespace Function
{
bool parse(Generator *gen,
           Node *node)
{
    return dale::Form::Function::parse(gen, node, NULL, NULL,
                                       Linkage::Null, 0);
}
}
}
}
}
