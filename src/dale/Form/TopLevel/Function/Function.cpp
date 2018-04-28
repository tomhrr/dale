#include "Function.h"
#include "../../../Node/Node.h"
#include "../../../Units/Units.h"
#include "../../Function/Function.h"
#include "Config.h"

namespace dale {
bool FormTopLevelFunctionParse(Units *units, Node *node,
                               const char *name) {
    return FormFunctionParse(units, node, name, NULL, Linkage::Null, 0);
}
}
