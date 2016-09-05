#include "Function.h"
#include "Config.h"
#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../Function/Function.h"

namespace dale
{
bool
FormTopLevelFunctionParse(Units *units, Node *node, const char *name)
{
    return FormFunctionParse(units, node, name, NULL, Linkage::Null, 0);
}
}
