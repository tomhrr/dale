#ifndef DALE_FORM_TYPE
#define DALE_FORM_TYPE

#include "../../Context/Context.h"
#include "../../Node/Node.h"
#include "../../Element/Type/Type.h"

namespace dale { 
class Generator;
namespace Form { namespace Type {
Element::Type *
parse(Generator *gen, Node *top, bool allow_anon_structs,
      bool allow_bitfields, bool allow_refs = false, 
      bool allow_retvals = false);
}}}

#endif
