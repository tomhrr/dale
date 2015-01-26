#include "Type.h"

#include "../STLUtils/STLUtils.h"
#include "../Utils/Utils.h"

#define DEBUG 0

#include <cstdio>
#include <cstring>

namespace dale
{
Type::Type()
{
    this->reset();
}

Type::Type(int new_base_type)
{
    this->reset();
    base_type = new_base_type;
}

Type::Type(Type *new_points_to)
{
    this->reset();
    points_to = new_points_to;
}

Type::~Type()
{
}

void
Type::reset(void)
{
    base_type       = 0;
    is_array        = false;
    array_size      = 0;
    points_to       = NULL;
    array_type      = NULL;
    bitfield_size   = 0;

    is_function     = false;
    return_type     = NULL;

    is_const        = false;
    is_reference    = false;
    is_retval       = false;
}

bool Type::isEqualTo(Type *other_type,
                     bool ignore_arg_constness)
{
    if (base_type != other_type->base_type) {
        return false;
    }

    if (is_const != other_type->is_const) {
        if (!(ignore_arg_constness && !is_const && other_type->is_const)) {
            return false;
        }
    }

    /* Bitfield size checking. */
    if (bitfield_size != other_type->bitfield_size) {
        if (DEBUG)  printf("Bitfield size mismatch: %d, %d\n",
                               bitfield_size,
                               other_type->bitfield_size);
        return false;
    } else {
        if (DEBUG)  printf("Bitfield sizes are fine: %d, %d\n",
                               bitfield_size,
                               other_type->bitfield_size);
    }

    /* Namespace checking. */
    if ((namespaces.size() == 0) ^ (other_type->namespaces.size() == 0)) {
        if (DEBUG)  printf("namespaces is set in one but not the other\n");
        if (DEBUG)  printf("(%p) (%p)\n", &(namespaces),
                                          &(other_type->namespaces));
        return false;
    } else {
        if (DEBUG)  printf("namespaces set in both\n");
    }

    if (namespaces.size()) {
        if (namespaces != other_type->namespaces) {
            return false;
        }
    }

    if (DEBUG)  printf("namespaces are fine, part 2\n");

    if ((struct_name.size()) ^ (other_type->struct_name.size())) {
        if (DEBUG)  printf("struct name is set in one but not the other\n");
        return false;
    } else {
        if (DEBUG)  printf("no struct name problems\n");
    }

    if (struct_name.size()) {
        if (struct_name != other_type->struct_name) {
            return false;
        }
    }

    if (DEBUG)  printf("no struct name problems, part 2\n");

    /* Array and array size checking. */

    if (is_array != other_type->is_array) {
        if (DEBUG)  printf("ERR: one is array, one is not\n");
        return false;
    }
    if (array_size != other_type->array_size) {
        if (DEBUG)  printf("ERR: array size is off\n");
        return false;
    }
    if (is_array) {
        if (!array_type || !other_type->array_type) {
            if (DEBUG)  printf("ERR: one has array type, one does not\n");
            return false;
        }
        return array_type->isEqualTo(other_type->array_type,
                                     ignore_arg_constness);
    }


    /* Function type checking. */

    if (is_function != other_type->is_function) {
        return false;
    }
    if (is_function) {
        if (!return_type->isEqualTo(other_type->return_type)) {
            return false;
        }
        return dale::stl::isEqualTo(
                   &(parameter_types),
                   &(other_type->parameter_types)
               );
    }

    /* Pointer checking. */

    if ((points_to == NULL) && (other_type->points_to != NULL)) {
        if (DEBUG)  printf("first is not pointer, second is pointer\n");
        return false;
    }
    if ((other_type->points_to == NULL) && (points_to != NULL)) {
        if (DEBUG)  printf("first is pointer, second is not pointer\n");
        return false;
    }
    if ((points_to == NULL) && (other_type->points_to == NULL)) {
        if (DEBUG)  printf("both are not pointers, all good\n");
        return true;
    }

    if (DEBUG)  printf("ok - checking the types to which these types point\n");
    return points_to->isEqualTo(other_type->points_to,
                                ignore_arg_constness);
}

bool
Type::canBeSetFrom(Type *value_type,
                   bool ignore_arg_constness)
{
    int iac = (ignore_arg_constness ? 1 : 0);
    int prev_const = value_type->is_const;
    value_type->is_const = 0;
    bool result =
        (points_to && value_type->points_to)
            ? value_type->isEqualTo(this, iac)
            : this->isEqualTo(value_type, iac);
    value_type->is_const = prev_const;
    return result;
}

bool
Type::canBePassedFrom(Type *value_type,
                      bool ignore_arg_constness)
{
    int prev_const = is_const;
    is_const = 0;
    bool result = canBeSetFrom(value_type, ignore_arg_constness);
    is_const = prev_const;
    return result;
}

/* todo: this doesn't handle namespaces! */
Node *Type::toNode(void)
{
    if (struct_name.size()) {
        Token *t = new Token(TokenType::String, 0,0,0,0);
        char tb[1024];
        tb[0] = '\0';
        if (namespaces.size()) {
            std::vector<std::string>::iterator iter =
                namespaces.begin();
            while (iter != namespaces.end()) {
                strcat(tb, (*iter).c_str());
                strcat(tb, ".");
                ++iter;
            }
        }
        strcat(tb, struct_name.c_str());
        std::string myns;
        if (getTypeMapEntry(tb, &myns)) {
            t->str_value.append(myns);
        } else {
            t->str_value.append(tb);
        }
        return new Node(t);
    }

    if (base_type) {
        Token *t = new Token(TokenType::String, 0,0,0,0);
        t->str_value.append(baseTypeToString(base_type));
        return new Node(t);
    }

    if (points_to) {
        std::vector<Node *> *nodes = new std::vector<Node*>;

        Token *t = new Token(TokenType::String, 0,0,0,0);
        t->str_value.append("p");
        nodes->push_back(new Node(t));

        Node *type = points_to->toNode();
        nodes->push_back(type);

        return new Node(nodes);
    }

    if (is_const) {
        std::vector<Node *> *nodes = new std::vector<Node*>;

        Token *t = new Token(TokenType::String, 0,0,0,0);
        t->str_value.append("const");
        nodes->push_back(new Node(t));

        Node *type = points_to->toNode();
        nodes->push_back(type);

        return new Node(nodes);
    }

    if (is_array) {
        std::vector<Node *> *nodes = new std::vector<Node*>;

        Token *t = new Token(TokenType::String, 0,0,0,0);
        t->str_value.append("array-of");
        nodes->push_back(new Node(t));

        char buf[100];
        sprintf(buf, "%d", (int) array_size);
        Token *size = new Token(TokenType::Int, 0,0,0,0);
        size->str_value.append(buf);

        Node *type = array_type->toNode();
        nodes->push_back(type);

        return new Node(nodes);
    }

    if (is_function) {
        std::vector<Node *> *nodes = new std::vector<Node*>;

        Token *t = new Token(TokenType::String, 0,0,0,0);
        t->str_value.append("fn");
        nodes->push_back(new Node(t));

        Node *ret_type = return_type->toNode();
        nodes->push_back(ret_type);

        std::vector<Node *> *pnodes = new std::vector<Node*>;

        std::vector<Type *>::iterator iter =
            parameter_types.begin();
        char c[] = "a";

        while (iter != parameter_types.end()) {
            std::vector<Node *> *anode = new std::vector<Node*>;
            Token *tnn = new Token(TokenType::String,0,0,0,0);
            tnn->str_value.append(c);
            c[0]++;
            anode->push_back(new Node(tnn));
            anode->push_back((*iter)->toNode());
            pnodes->push_back(new Node(anode));
            ++iter;
        }

        nodes->push_back(new Node(pnodes));

        return new Node(nodes);
    }

    return NULL;
}

void Type::toString(std::string *str)
{
    if (is_const) {
        str->append("(const ");
        is_const = 0;
        toString(str);
        str->append(")");
        is_const = 1;
        return;
    }

    if (is_reference) {
        str->append("(ref ");
        is_reference = 0;
        toString(str);
        str->append(")");
        is_reference = 1;
        return;
    }

    if (struct_name.size()) {
        char tb[1024];
        tb[0] = '\0';
        if (namespaces.size()) {
            std::vector<std::string>::iterator iter =
                namespaces.begin();
            while (iter != namespaces.end()) {
                strcat(tb, (*iter).c_str());
                strcat(tb, ".");
                ++iter;
            }
        }
        strcat(tb, struct_name.c_str());
        std::string myns;
        if (getTypeMapEntry(tb, &myns)) {
            str->append(myns);
        } else {
            str->append(tb);
        }
        return;
    }

    if (bitfield_size) {
        char buf[255];
        sprintf(buf, "%d", bitfield_size);
        str->append("(bf ")
        .append(baseTypeToString(base_type))
        .append(" ")
        .append(buf)
        .append(")");
        return;
    }

    if (base_type) {
        str->append(baseTypeToString(base_type));
        return;
    }

    if (points_to) {
        str->append("(p ");
        points_to->toString(str);
        str->append(")");
        return;
    }

    if (is_array) {
        str->append("(array-of ");
        char buf[100];
        sprintf(buf, "%d", (int) array_size);
        str->append(buf);
        str->append(" ");
        array_type->toString(str);
        str->append(")");
        return;
    }

    if (is_function) {
        str->append("(fn ");
        return_type->toString(str);
        str->append(" (");

        std::vector<Type *>::iterator iter =
            parameter_types.begin();
        while (iter != parameter_types.end()) {
            (*iter)->toString(str);
            ++iter;
        }

        str->append(")");
        return;
    }

    return;
}

Type *Type::makeCopy(void)
{
    Type *new_type = new Type();

    new_type->base_type     = base_type;
    new_type->is_array      = is_array;
    new_type->array_size    = array_size;
    new_type->is_function   = is_function;
    new_type->bitfield_size = bitfield_size;
    new_type->is_const      = is_const;
    new_type->is_reference  = is_reference;
    new_type->is_retval     = is_retval;

    if (points_to) {
        new_type->points_to = points_to->makeCopy();
        return new_type;
    }

    if (array_type) {
        new_type->array_type = array_type->makeCopy();
        return new_type;
    }

    if (is_function) {
        new_type->return_type = return_type->makeCopy();
        std::vector<Type *>::iterator iter = parameter_types.begin();
        while (iter != parameter_types.end()) {
            new_type->parameter_types.push_back((*iter)->makeCopy());
            ++iter;
        }
        return new_type;
    }

    if (struct_name.size()) {
        new_type->struct_name = struct_name;
    }
    if (namespaces.size()) {
        new_type->namespaces = namespaces;
    }

    return new_type;
}

void Type::toSymbolString(std::string *to)
{
    if (points_to) {
        to->append("P");
        points_to->toSymbolString(to);
        return;
    }

    if (is_array) {
        to->append("A");
        char buf[100];
        sprintf(buf, "%d", (int) array_size);
        to->append(buf);
        array_type->toSymbolString(to);
        return;
    }

    if (base_type) {
        char c = baseTypeToSymbolChar(base_type);
        if (c == '?') {
            fprintf(stderr, "Invalid base type (%d).\n", base_type);
            abort();
        }
        to->push_back(c);
        return;
    }

    if (struct_name.size()) {
        to->append("Z");
        if (namespaces.size()) {
            to->append("N");
            std::vector<std::string>::iterator iter;
            iter = namespaces.begin();
            while (iter != namespaces.end()) {
                int len = (*iter).length();
                char num[255];
                sprintf(num, "%d", len);
                to->append(num);
                to->append((*iter));
                ++iter;
            }
            to->append("E");
        }
        int len = struct_name.size();
        char num[255];
        sprintf(num, "%d", len);
        to->append(num);
        encodeStandard(&struct_name, to);
        return;
    }

    if (is_function) {
        to->append("F");
        return_type->toSymbolString(to);
        std::vector<Type *>::iterator iter =
            parameter_types.begin();
        while (iter != parameter_types.end()) {
            (*iter)->toSymbolString(to);
            ++iter;
        }
        to->append("E");
        return;
    }

    if (DEBUG)  printf("ERROR: cannot yet handle encstr for this type\n");
}

bool Type::isIntegerType(void)
{
    return baseTypeIsIntegerType(base_type);
}

bool Type::isSignedIntegerType(void)
{
    return baseTypeIsSignedIntegerType(base_type);
}

int Type::getIntegerSize(void)
{
    if (bitfield_size) {
        return 1000 + bitfield_size;
    }
    return integerTypeToSize(base_type);
}

int Type::getFloatingPointRelativeSize(void)
{
    return floatingPointTypeToRelativeSize(base_type);
}

bool Type::isFloatingPointType(void)
{
    return baseTypeIsFloatingPointType(base_type);
}

bool Type::isVarArgs(void)
{
    if (parameter_types.size() == 0) {
        return false;
    }

    Type *back = parameter_types.back();

    return (back->base_type == BaseType::VarArgs);
}

int Type::numberOfRequiredArgs(void)
{
    if (parameter_types.size() == 0) {
        return 0;
    }

    unsigned int num_of_args = parameter_types.size();

    if (isVarArgs()) {
        num_of_args -= 1;
    }

    return num_of_args;
}
}
