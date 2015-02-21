#include "Function.h"
#include "Config.h"
#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../Function/Function.h"

namespace dale
{
bool
FormTopLevelFunctionParse(Units *units, Node *node)
{
    return FormFunctionParse(units, node, NULL, NULL,
                                       Linkage::Null, 0);
}
}
