#ifndef DALE_FORM_TYPE
#define DALE_FORM_TYPE

#include "../../Context/Context.h"
#include "../../Node/Node.h"
#include "../../Type/Type.h"
#include "../../Units/Units.h"

namespace dale { 
class Generator;
Type *
FormTypeParse(Units *units, Node *top, bool allow_anon_structs,
      bool allow_bitfields, bool allow_refs = false, 
      bool allow_retvals = false);
}

#endif
