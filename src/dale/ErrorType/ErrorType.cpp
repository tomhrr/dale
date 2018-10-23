#include "ErrorType.h"

#include <cstdio>
#include <cstdlib>

namespace dale {
const char *errorInstanceToString(int error_instance) {
    const char *ret;
    switch (error_instance) {
        case ErrorInst::Null:
            ret = "Null";
            break;
        case ErrorInst::ExpectedList:
            ret = "expected list";
            break;
        case ErrorInst::ExpectedLeftParen:
            ret = "expected left parenthesis";
            break;
        case ErrorInst::MissingLeftParen:
            ret = "missing left parenthesis";
            break;
        case ErrorInst::MissingRightParen:
            ret = "missing right parenthesis";
            break;
        case ErrorInst::UnterminatedStringLiteral:
            ret = "unterminated string literal";
            break;
        case ErrorInst::InvalidInteger:
            ret = "invalid integer";
            break;
        case ErrorInst::InvalidFloatingPointNumber:
            ret = "invalid floating point number";
            break;
        case ErrorInst::OnlyListsAtTopLevel:
            ret = "top level elements must be lists";
            break;
        case ErrorInst::NoEmptyLists:
            ret = "lists cannot be empty";
            break;
        case ErrorInst::FirstListElementMustBeAtom:
            ret = "first list element must be atom";
            break;
        case ErrorInst::FirstListElementMustBeAtomOrMacroOrFP:
            ret =
                "first list element must be atom, or macro call, or "
                "evaluate "
                "to a function pointer";
            break;
        case ErrorInst::FirstListElementMustBeSymbol:
            ret = "first list element must be symbol";
            break;
        case ErrorInst::NoEmptyDo:
            ret = "'do' form must not be empty";
            break;
        case ErrorInst::IncorrectNumberOfArgs:
            ret = "'%s' requires %s arguments (got %s instead)";
            break;
        case ErrorInst::IncorrectMinimumNumberOfArgs:
            ret =
                "'%s' requires at least %s arguments "
                "(got %s instead)";
            break;
        case ErrorInst::IncorrectMaximumNumberOfArgs:
            ret =
                "'%s' takes at most %s arguments "
                "(got %s instead)";
            break;
        case ErrorInst::IncorrectArgType:
            ret = "'%s' requires %s for argument %s (got %s instead)";
            break;
        case ErrorInst::IncorrectSingleParameterType:
            ret = "single parameter type must be %s (got %s instead)";
            break;
        case ErrorInst::IncorrectParameterTypeNumberOfArgs:
            ret =
                "parameter type requires %s arguments "
                "(got %s instead)";
            break;
        case ErrorInst::LabelNotInScope:
            ret = "label not in scope: '%s'";
            break;
        case ErrorInst::TypeNotInScope:
            ret = "type not in scope: '%s'";
            break;
        case ErrorInst::MacroNotInScope:
            ret = "macro not in scope: '%s'";
            break;
        case ErrorInst::NamespaceNotInScope:
            ret = "namespace not in scope: '%s'";
            break;
        case ErrorInst::OverloadedFunctionOrMacroNotInScope:
            ret =
                "overloaded function/macro not in scope: '%s' "
                "(parameters are %s)";
            break;
        case ErrorInst::OverloadedFunctionOrMacroNotInScopeWithClosest:
            ret =
                "overloaded function/macro not in scope: '%s' "
                "(parameters are %s, closest candidate expects %s)";
            break;
        case ErrorInst::
            OverloadedFunctionOrMacroNotInScopeWithClosestFirstMatches:
            ret =
                "overloaded function/macro not in scope: '%s' "
                "(parameters are %s, closest candidate expects %s)";
            break;
        case ErrorInst::NotInScope:
            ret = "not in scope: '%s'";
            break;
        case ErrorInst::VariableNotInScope:
            ret = "variable not in scope or cannot be used as value: '%s'";
            break;
        case ErrorInst::UnexpectedElement:
            ret = "expected %s for %s (got %s instead)";
            break;
        case ErrorInst::VoidMustBeTheOnlyParameter:
            ret = "'void' must be the only parameter";
            break;
        case ErrorInst::VarArgsMustBeLastParameter:
            ret = "the varargs specifier must be the last parameter";
            break;
        case ErrorInst::VarArgsNotPermittedForMacros:
            ret =
                "the varargs specifier may not be used in macros (use "
                "'rest' "
                "instead)";
            break;
        case ErrorInst::TypeNotAllowedInStruct:
            ret = "'%s' fields are not allowed in structs";
            break;
        case ErrorInst::CannotInitialiseExternVar:
            ret = "'extern' variables cannot be initialised";
            break;
        case ErrorInst::InvalidType:
            ret = "invalid type";
            break;
        case ErrorInst::CannotParseLiteral:
            ret = "parsing literals of type %s is not supported";
            break;
        case ErrorInst::RedefinitionOfVariable:
            ret =
                "variable '%s' has already been defined in "
                "this scope";
            break;
        case ErrorInst::RedeclarationOfStruct:
            ret =
                "struct '%s' has already been defined in "
                "this scope";
            break;
        case ErrorInst::RedeclarationOfLabel:
            ret =
                "label '%s' has already been defined in "
                "this scope";
            break;
        case ErrorInst::RedeclarationOfFunctionOrMacro:
            ret =
                "function/macro '%s' has already been defined in "
                "this scope";
            break;
        case ErrorInst::NonPointerFunctionParameter:
            ret =
                "all function parameters must be pointers to "
                "functions";
            break;
        case ErrorInst::ExternCInNamespace:
            ret =
                "functions with extern-c linkage cannot be "
                "defined within a namespace";
            break;
        case ErrorInst::IncorrectType:
            ret =
                "expected expression with type %s (got type "
                "%s instead)";
            break;
        case ErrorInst::IncorrectReturnType:
            ret =
                "expected return expression with type %s (got type "
                "%s instead)";
            break;
        case ErrorInst::CannotDereferenceNonPointer:
            ret = "cannot dereference non-pointer (type is %s)";
            break;
        case ErrorInst::CannotDereferenceVoidPointer:
            ret = "cannot dereference void pointer (type is %s)";
            break;
        case ErrorInst::CannotDereferencePointerInContext:
            ret = "cannot dereference pointer in this context";
            break;
        case ErrorInst::OverloadedFunctionAddressWithoutTypes:
            ret =
                "cannot take address of overloaded function "
                "without also providing the function's types";
            break;
        case ErrorInst::NoSuchFileOrDirectory:
            ret = "%s: no such file or directory";
            break;
        case ErrorInst::FileError:
            ret = "%s: %s";
            break;
        case ErrorInst::InvalidCast:
            ret = "unable to cast from type %s to type %s";
            break;
        case ErrorInst::FieldDoesNotExistInStruct:
            ret = "field '%s' does not exist in struct %s";
            break;
        case ErrorInst::OnlyVarAndStructPermitted:
            ret =
                "'def' within a function may only be used "
                "for var and struct forms";
            break;
        // This is very much a 'last resort' error.
        case ErrorInst::UnableToParseForm:
            ret = "unable to parse form";
            break;
        case ErrorInst::InvalidChar:
            ret = "'%s' is not a valid char literal";
            break;
        case ErrorInst::TypeNotSupportedForMacro:
            ret = "type %s cannot be used for macro parameters";
            break;
        case ErrorInst::GotNullPointerFromMacroCall:
            ret = "macro '%s' returned a null pointer";
            break;
        case ErrorInst::UnableToParseInteger:
            ret = "unable to parse integer %s";
            break;
        case ErrorInst::UnableToParseIntegerNoString:
            ret = "unable to parse integer %s";
            break;
        case ErrorInst::UnableToConvertTypeToLLVMType:
            ret = "unable to convert type %s to LLVM type";
            break;
        case ErrorInst::DNodeHasNoString:
            ret =
                "DNode returned by macro is an atom but "
                "has no string";
            break;
        case ErrorInst::DNodeIsNeitherTokenNorList:
            ret =
                "DNode returned by macro is neither token "
                "nor list";
            break;
        case ErrorInst::NodeIsNeitherTokenNorList:
            ret = "internal node is neither token nor list";
            break;
        case ErrorInst::TypeIsNotFirstClass:
            ret =
                "type %s cannot be used, because it is not "
                "a first-class type";
            break;
        case ErrorInst::ExternalError:
            ret = "%s";
            break;
        case ErrorInst::NamespaceNotInContext:
            ret = "namespace '%s' does not exist in context";
            break;
        case ErrorInst::CannotDeactivateInactiveNamespace:
            ret = "cannot deactivate inactive namespace '%s'";
            break;
        case ErrorInst::CannotDeactivateNonLastNamespace:
            ret =
                "cannot deactivate namespace '%s'; it is not "
                "the most recently used namespace";
            break;
        case ErrorInst::ArraysCannotBeFunctionParameters:
            ret =
                "arrays cannot be function parameters; use a "
                "pointer instead";
            break;
        case ErrorInst::IncorrectNumberOfArrayElements:
            ret =
                "incorrect number of array elements (got "
                "%s, expected %s)";
            break;
        case ErrorInst::CanOnlyIndexIntoPointersAndArrays:
            ret =
                "can only index into pointers and arrays (type "
                "of value is %s)";
            break;
        case ErrorInst::ExternCXorOtherLinkageForName:
            ret =
                "cannot have extern-c function for name "
                "as well as extern/intern functions for "
                "name: %s";
            break;
        case ErrorInst::ReturnTypesCannotBeArrays:
            ret =
                "return types cannot be arrays (use a pointer "
                "instead)";
            break;
        case ErrorInst::CannotTakeAddressOfNonLvalue:
            ret = "cannot take address of non-lvalue";
            break;
        case ErrorInst::FunctionNotInScope:
            ret =
                "function not in scope: '%s' (parameters are %s, "
                "function expects %s)";
            break;
        case ErrorInst::MustHaveInitialiserForImpliedType:
            ret =
                "must have initialiser for variable with implied "
                "type";
            break;
        case ErrorInst::MustHaveInitialiserForConstType:
            ret =
                "must have initialiser for variable with const "
                "type";
            break;
        case ErrorInst::MustHaveInitialiserForType:
            ret = "must have initialiser for variable of this type";
            break;
        case ErrorInst::StructContainsPadding:
            ret = "struct contains padding, may cause problems";
            break;
        case ErrorInst::UnableToResolvePointerAddress:
            ret =
                "unable to resolve pointer address in struct "
                "declaration";
            break;
        case ErrorInst::TypeNotSupported:
            ret = "type %s is not supported on this platform";
            break;
        case ErrorInst::ValueWillNeverBeNull:
            ret = "this value will never be null";
            break;
        case ErrorInst::NoCoreFormNameInMacro:
            ret = "core form names cannot be used as macro names";
            break;
        case ErrorInst::ThisCoreFormCannotBeOverridden:
            ret = "this core form cannot be overridden";
            break;
        case ErrorInst::InvalidAttribute:
            ret = "invalid attribute";
            break;
        case ErrorInst::AttributesOfDeclAndDefAreDifferent:
            ret =
                "attributes for the declaration of function '%s' "
                "do not match the attributes for the definition";
            break;
        case ErrorInst::ApplyMustTakePointerToStructAsFirstArgument:
            ret =
                "the 'apply' function must take a pointer to "
                "the relevant struct as its first argument";
            break;
        case ErrorInst::HasBothExternAndInitialiser:
            ret =
                "a function-scoped variable cannot have extern "
                "linkage as well as an initialiser";
            break;
        case ErrorInst::CannotInstantiateOpaqueStruct:
            ret = "cannot instantiate opaque struct";
            break;
        case ErrorInst::ModuleDoesNotProvideForms:
            ret =
                "module '%s' does not provide the following forms: %s";
            break;
        case ErrorInst::UnableToLoadModule:
            ret = "unable to load module '%s'";
            break;
        case ErrorInst::InvalidModuleName:
            ret =
                "module name '%s' is invalid: module names may "
                "only comprise alphanumeric characters, periods, "
                "hyphens and underscores";
            break;
        case ErrorInst::BitfieldMustHaveIntegerType:
            ret = "bitfields must have an integer type";
            break;
        case ErrorInst::ZeroLengthGlobalArraysAreUnsupported:
            ret = "zero-length global arrays are currently unsupported";
            break;
        case ErrorInst::GotoWillCrossDeclaration:
            ret = "this goto will cross a declaration";
            break;
        case ErrorInst::FunctionHasSameParamsAsMacro:
            ret =
                "function '%s' has same parameters as an "
                "existing macro of that name";
            break;
        case ErrorInst::MacroHasSameParamsAsFunction:
            ret =
                "macro '%s' has same parameters as an "
                "existing function of that name";
            break;
        case ErrorInst::CannotModifyConstVariable:
            ret = "cannot modify const variable";
            break;
        case ErrorInst::SetfOverridesMustReturnBool:
            ret = "setf overrides must return bool";
            break;
        case ErrorInst::CopyDisabled:
            ret = "copying is disabled for this type";
            break;
        case ErrorInst::RefsNotPermittedHere:
            ret = "reference types not permitted in this context";
            break;
        case ErrorInst::RetvalsNotPermittedHere:
            ret = "retval types not permitted in this context";
            break;
        case ErrorInst::CTOFromNonCTO:
            ret =
                "CTO functions may only be called from macros or other "
                "CTO "
                "functions";
            break;
        case ErrorInst::CTOAddrFromNonCTO:
            ret =
                "can only take address of CTO function from macro or "
                "other CTO "
                "function";
            break;
        case ErrorInst::CTOAnonymousFromNonCTO:
            ret =
                "can only use anonymous CTO functions from macros or "
                "other CTO "
                "functions";
            break;
        case ErrorInst::OnlyOneModuleFormPermitted:
            ret = "a 'module' form may only appear once";
            break;
        case ErrorInst::MacroExpansionError:
            ret = "macro expansion error (see previous)";
            break;
        case ErrorInst::ArityForExternCOnly:
            ret =
                "arity may only be called with extern-c function names";
            break;
        case ErrorInst::NoRefsInExternC:
            ret =
                "extern-c functions may not have reference parameters";
            break;
        case ErrorInst::UnsupportedEvalExpressionType:
            ret = "eval-expression does not support this type";
            break;
        case ErrorInst::RedeclarationOfDifferentKind:
            ret = "'%s' redeclared as different kind of symbol";
            break;
        case ErrorInst::MustHaveConstantInitialiser:
            ret =
                "must have constant initialiser for function-scoped "
                "intern variable";
            break;
        default:
            ret = "(Unknown)";
    }
    return ret;
}

int errorInstanceToType(int error_instance) {
    int type;
    switch (error_instance) {
        case ErrorInst::Null:
            type = ErrorType::Diagnostic;
            break;
        case ErrorInst::StructContainsPadding:
            type = ErrorType::Warning;
            break;
        default:
            type = ErrorType::Error;
    }
    return type;
}

const char *errorTypeToString(int error_type) {
    const char *str;
    switch (error_type) {
        case ErrorType::Error:
            str = "error";
            break;
        case ErrorType::Warning:
            str = "warning";
            break;
        case ErrorType::Diagnostic:
            str = "diagnostic";
            break;
        default:
            str = "unknown";
    }
    return str;
}
}
