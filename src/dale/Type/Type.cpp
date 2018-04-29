#include "Type.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "../STL/STL.h"
#include "../Utils/Utils.h"

namespace dale {
Type::Type() { this->reset(); }

Type::Type(int new_base_type) {
    this->reset();
    base_type = new_base_type;
}

Type::Type(Type *new_points_to) {
    this->reset();
    points_to = new_points_to;
}

Type::~Type() {}

void Type::reset() {
    base_type = 0;
    is_array = false;
    array_size = 0;
    points_to = NULL;
    array_type = NULL;
    bitfield_size = 0;

    is_function = false;
    return_type = NULL;

    is_const = false;
    is_reference = false;
    is_rvalue_reference = false;
    is_retval = false;
}

bool Type::isEqualTo(Type *other_type, bool ignore_arg_constness,
                     bool checking_passed_from) {
    if (is_reference && other_type->is_rvalue_reference) {
        return false;
    }

    if (is_rvalue_reference && other_type->is_reference) {
        return false;
    }

    if (base_type != other_type->base_type) {
        return false;
    }

    if (is_const != other_type->is_const) {
        if (!(ignore_arg_constness && !is_const &&
              other_type->is_const)) {
            return false;
        }
    }

    if (bitfield_size != other_type->bitfield_size) {
        return false;
    }

    if ((namespaces.size() == 0) ^
        (other_type->namespaces.size() == 0)) {
        return false;
    }

    if (namespaces.size()) {
        if (namespaces != other_type->namespaces) {
            return false;
        }
    }

    if ((struct_name.size()) ^ (other_type->struct_name.size())) {
        return false;
    }

    if (struct_name.size()) {
        if (struct_name != other_type->struct_name) {
            return false;
        }
    }

    if (is_array != other_type->is_array) {
        return false;
    }
    if (array_size != other_type->array_size) {
        return false;
    }
    if (is_array) {
        if (!array_type || !other_type->array_type) {
            return false;
        }
        return array_type->isEqualTo(other_type->array_type,
                                     ignore_arg_constness,
                                     checking_passed_from);
    }

    if (is_function != other_type->is_function) {
        return false;
    }
    if (is_function) {
        if (!return_type->isEqualTo(other_type->return_type)) {
            return false;
        }
        std::vector<Type *>::iterator
            b = parameter_types.begin(),
            e = parameter_types.end(),
            ob = other_type->parameter_types.begin(),
            oe = other_type->parameter_types.end();
        for (; (b != e) && (ob != oe); ++b, ++ob) {
            if (checking_passed_from) {
                if (!(*b)->canBePassedFrom(*ob, true)) {
                    return false;
                }
            } else {
                if (!(*b)->isEqualTo(*ob)) {
                    return false;
                }
            }
        }
        if (!((b == e) && (ob == oe))) {
            return false;
        }
        return true;
    }

    if ((points_to == NULL) && (other_type->points_to != NULL)) {
        return false;
    }
    if ((other_type->points_to == NULL) && (points_to != NULL)) {
        return false;
    }
    if ((points_to == NULL) && (other_type->points_to == NULL)) {
        return true;
    }

    return points_to->isEqualTo(other_type->points_to,
                                ignore_arg_constness,
                                checking_passed_from);
}

bool Type::canBeSetFrom(Type *value_type, bool ignore_arg_constness,
                        bool checking_passed_from) {
    int iac = (ignore_arg_constness ? 1 : 0);
    int prev_const = value_type->is_const;
    value_type->is_const = 0;
    bool result =
        (points_to && value_type->points_to)
            ? value_type->isEqualTo(this, iac, checking_passed_from)
            : this->isEqualTo(value_type, iac, checking_passed_from);
    value_type->is_const = prev_const;
    return result;
}

bool Type::canBePassedFrom(Type *value_type,
                           bool ignore_arg_constness) {
    int prev_const = is_const;
    is_const = false;
    bool result = canBeSetFrom(value_type, ignore_arg_constness, true);
    is_const = prev_const;
    return result;
}

Node *Type::toNode() {
    if (struct_name.size()) {
        Token *t = new Token(TokenType::String);
        std::vector<std::string>::iterator iter = namespaces.begin();
        while (iter != namespaces.end()) {
            t->str_value.append((*iter).c_str());
            t->str_value.append(".");
            ++iter;
        }
        t->str_value.append(struct_name.c_str());
        std::string myns;
        if (getTypeMapEntry(t->str_value.c_str(), &myns)) {
            t->str_value.append(myns);
        }
        return new Node(t);
    }

    if (is_const) {
        std::vector<Node *> *nodes = new std::vector<Node *>;

        Token *t = new Token(TokenType::String);
        t->str_value.append("const");
        nodes->push_back(new Node(t));

        is_const = false;
        Node *type = toNode();
        is_const = true;
        nodes->push_back(type);

        return new Node(nodes);
    }

    if (points_to) {
        std::vector<Node *> *nodes = new std::vector<Node *>;

        Token *t = new Token(TokenType::String);
        t->str_value.append("p");
        nodes->push_back(new Node(t));

        Node *type = points_to->toNode();
        nodes->push_back(type);

        return new Node(nodes);
    }

    if (is_array) {
        std::vector<Node *> *nodes = new std::vector<Node *>;

        Token *t = new Token(TokenType::String);
        t->str_value.append("array-of");
        nodes->push_back(new Node(t));

        Token *size = new Token(TokenType::Int);
        appendInt(&(size->str_value), array_size);

        Node *type = array_type->toNode();
        nodes->push_back(type);

        return new Node(nodes);
    }

    if (is_function) {
        std::vector<Node *> *nodes = new std::vector<Node *>;

        Token *t = new Token(TokenType::String);
        t->str_value.append("fn");
        nodes->push_back(new Node(t));

        Node *ret_type = return_type->toNode();
        nodes->push_back(ret_type);

        std::vector<Node *> *pnodes = new std::vector<Node *>;

        std::vector<Type *>::iterator iter = parameter_types.begin();
        char c[] = "a";

        while (iter != parameter_types.end()) {
            std::vector<Node *> *anode = new std::vector<Node *>;
            Token *tnn = new Token(TokenType::String);
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

    if (base_type) {
        Token *t = new Token(TokenType::String);
        t->str_value.append(baseTypeToString(base_type));
        return new Node(t);
    }

    return NULL;
}

void Type::toString(std::string *str) {
    if (is_reference) {
        str->append("(ref ");
        is_reference = false;
        toString(str);
        str->append(")");
        is_reference = true;
        return;
    }

    if (is_rvalue_reference) {
        str->append("(rv-ref ");
        is_rvalue_reference = false;
        toString(str);
        str->append(")");
        is_rvalue_reference = true;
        return;
    }

    if (is_const) {
        str->append("(const ");
        is_const = false;
        toString(str);
        str->append(")");
        is_const = true;
        return;
    }

    if (struct_name.size()) {
        std::string name;
        if (namespaces.size()) {
            std::vector<std::string>::iterator iter =
                namespaces.begin();
            while (iter != namespaces.end()) {
                name.append((*iter).c_str());
                name.append(".");
                ++iter;
            }
        }
        name.append(struct_name.c_str());
        std::string myns;
        if (getTypeMapEntry(name.c_str(), &myns)) {
            str->append(myns);
        } else {
            str->append(name);
        }
        return;
    }

    if (bitfield_size) {
        str->append("(bf ")
            .append(baseTypeToString(base_type))
            .append(" ");
        appendInt(str, bitfield_size);
        str->append(")");
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
        appendInt(str, array_size);
        str->append(" ");
        array_type->toString(str);
        str->append(")");
        return;
    }

    if (is_function) {
        str->append("(fn ");
        return_type->toString(str);
        str->append(" (");

        for (std::vector<Type *>::iterator pb = parameter_types.begin(),
                                           pe = parameter_types.end();
             pb != pe; ++pb) {
            (*pb)->toString(str);
            if ((pb + 1) != pe) {
                str->append(" ");
            }
        }

        str->append(")");
        return;
    }

    return;
}

Type *Type::makeCopy() {
    Type *new_type = new Type();

    new_type->base_type = base_type;
    new_type->is_array = is_array;
    new_type->array_size = array_size;
    new_type->is_function = is_function;
    new_type->bitfield_size = bitfield_size;
    new_type->is_const = is_const;
    new_type->is_reference = is_reference;
    new_type->is_rvalue_reference = is_rvalue_reference;
    new_type->is_retval = is_retval;

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

void Type::toSymbolString(std::string *to) {
    if (points_to) {
        to->append("P");
        points_to->toSymbolString(to);
        return;
    }

    if (is_array) {
        to->append("A");
        appendInt(to, array_size);
        array_type->toSymbolString(to);
        return;
    }

    if (is_reference) {
        to->append("R");
        is_reference = false;
        toSymbolString(to);
        is_reference = true;
        return;
    }

    if (is_rvalue_reference) {
        to->append("V");
        is_rvalue_reference = false;
        toSymbolString(to);
        is_rvalue_reference = true;
        return;
    }

    if (base_type) {
        char c = baseTypeToSymbolChar(base_type);
        assert((c != '?') && "invalid base type");
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
                appendInt(to, ((*iter).length()));
                to->append((*iter));
                ++iter;
            }
            to->append("E");
        }
        appendInt(to, struct_name.size());
        encodeStandard(&struct_name, to);
        return;
    }

    if (is_function) {
        to->append("F");
        return_type->toSymbolString(to);
        std::vector<Type *>::iterator iter = parameter_types.begin();
        while (iter != parameter_types.end()) {
            (*iter)->toSymbolString(to);
            ++iter;
        }
        to->append("E");
        return;
    }

    assert(false && "unable to encode this type as a symbol string");
    abort();
}

bool Type::isIntegerType() { return baseTypeIsIntegerType(base_type); }

bool Type::isSignedIntegerType() {
    return baseTypeIsSignedIntegerType(base_type);
}

int Type::getIntegerSize() {
    if (bitfield_size) {
        return 1000 + bitfield_size;
    }
    return integerTypeToSize(base_type);
}

int Type::getFloatingPointRelativeSize() {
    return floatingPointTypeToRelativeSize(base_type);
}

bool Type::isFloatingPointType() {
    return baseTypeIsFloatingPointType(base_type);
}

bool Type::isVarArgs() {
    if (parameter_types.size() == 0) {
        return false;
    }

    Type *back = parameter_types.back();

    return (back->base_type == BaseType::VarArgs);
}

int Type::numberOfRequiredArgs() {
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
