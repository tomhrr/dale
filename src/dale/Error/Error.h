#ifndef DALE_ERROR
#define DALE_ERROR

#include <string>
#include <vector>
#include "../Node/Node.h"
#include "../Position/Position.h"

namespace dale
{
namespace ErrorType
{
enum
{
    Error = 1,
    Warning,
    Diagnostic,
    Debug,
    Internal
};
}

namespace ErrorInst
{
namespace Lexer
{
enum
{
    Null = 1,
    UnterminatedStringLiteral,
    InvalidInteger,
    InvalidFloatingPointNumber
};
}

namespace Parser
{
enum
{
    Null = 1001,
    MissingLeftParen,
    MissingRightParen,
    ExpectedLeftParen
};
}

namespace Generator
{
enum
{
    Null = 2001,

    CannotRemoveInternalFunction,
    CannotRemoveInternalVariable,
    CannotFindSymbolForMacro,
    CannotCreateExecutionEngine,
    CannotLoadLibrary,
    CannotLinkModules,

    OnlyListsAtTopLevel,
    NoEmptyLists,
    FirstListElementMustBeAtom,
    FirstListElementMustBeSymbol,
    NoEmptyDo,
    CannotOnceTheLastOpenFile,
    IncorrectNumberOfArgs,
    IncorrectMinimumNumberOfArgs,
    IncorrectMaximumNumberOfArgs,
    IncorrectArgType,
    IncorrectSingleParameterType,
    IncorrectParameterTypeNumberOfArgs,
    OverloadedFunctionOrMacroNotInScope,
    OverloadedFunctionOrMacroNotInScopeWithClosest,
    NamespaceNotInScope,
    TypeNotInScope,
    MacroNotInScope,
    FunctionNotInScope,
    VariableNotInScope,
    NotInScope,
    UnexpectedElement,
    VoidMustBeTheOnlyParameter,
    VarArgsMustBeLastParameter,
    TypeNotAllowedInStruct,
    CannotInitialiseExternVar,
    InvalidType,
    CannotParseLiteral,
    RedeclarationOfEnum,
    RedeclarationOfEnumElement,
    RedeclarationOfStruct,
    RedefinitionOfVariable,
    RedeclarationOfLabel,
    RedeclarationOfFunctionOrMacro,
    NonPointerFunctionParameter,
    ExternCInNamespace,
    IncorrectType,
    IncorrectReturnType,
    CannotDereferenceNonPointer,
    CannotDereferenceVoidPointer,
    OverloadedFunctionAddressWithoutTypes,
    NoSuchFileOrDirectory,
    FileError,
    InvalidCast,
    FieldDoesNotExistInStruct,
    EnumValueDoesNotExist,
    IfBranchesHaveDifferentTypes,
    OnlyVarPermitted,
    UnableToParseForm,
    InvalidChar,
    TypeNotSupportedForMacro,
    GotNullPointerFromMacroCall,
    LabelNotInScope,
    UnableToParseInteger,
    UnableToConvertTypeToLLVMType,
    TypeIsNotFirstClass,
    ArraysCannotBeFunctionParameters,
    IncorrectNumberOfArrayElements,
    CanOnlyIndexIntoPointersAndArrays,
    EnumTypeMustBeInteger,
    ExternCXorOtherLinkageForName,
    ReturnTypesCannotBeArrays,
    CannotTakeAddressOfNonLvalue,
    MustHaveInitialiserForImpliedType,
    MustHaveInitialiserForConstType,
    MustHaveInitialiserForType,
    NonNullPointerInGlobalStructDeclaration,
    TypeNotSupported,
    ValueWillNeverBeNull,
    NoCoreFormNameInMacro,
    ThisCoreFormCannotBeOverridden,
    InvalidAttribute,
    AttributesOfDeclAndDefAreDifferent,
    ApplyMustTakePointerToStructAsFirstArgument,
    HasBothExternAndInitialiser,
    CannotInstantiateOpaqueStruct,
    ModuleDoesNotProvideForms,
    UnableToLoadModule,
    BitfieldMustHaveIntegerType,
    GotoWillCrossDeclaration,
    FunctionHasSameParamsAsMacro,
    MacroHasSameParamsAsFunction,
    ZeroLengthGlobalArraysAreUnsupported,
    CannotModifyConstVariable,
    SetfOverridesMustReturnBool,

    /* these two should print the macro call that caused
     * them, if applicable */
    DNodeHasNoString,
    DNodeIsNeitherTokenNorList,

    /* internal errors */
    NodeIsNeitherTokenNorList,
    NamespaceNotInContext,
    CannotDeactivateInactiveNamespace,
    CannotDeactivateNonLastNamespace,
    InvalidModuleName,

    /* for errors reported via code */
    ExternalError,

    /* warnings */
    StructContainsPadding
};
}
}

class Error
{
public:
    int       instance;
    Position  begin;
    Position  end;
    Position  macro_begin;
    Position  macro_end;
    const char *filename;
    std::vector<std::string> *arg_strings;

    Error(const char *filename,
          int new_instance,
          int begin_line_number,
          int begin_column_number,
          int end_line_number,
          int end_column_number);
    Error(int new_instance,
          Node *node);
    Error(int new_instance,
          Node *node,
          const char *str1);
    Error(int new_instance,
          Node *node,
          int str1,
          int str2);
    Error(int new_instance,
          Node *node,
          const char *str1,
          int str2,
          int str3);
    Error(int new_instance,
          Node *node,
          const char *str1,
          const char *str2);
    Error(int new_instance,
          Node *node,
          const char *str1,
          const char *str2,
          const char *str3);
    Error(int new_instance,
          Node *node,
          const char *str1,
          const char *str2,
          const char *str3,
          const char *str4);
    ~Error();
    /* See constructor notes for Token - these classes are
     * also pretty similar and could possibly be refactored
     * into a single class. */
    const char *getPrintfString(void);
    int getType(void);
    void addArgString(std::string *str);
    void addArgString(const char *str);
    void toString(std::string *str);
};
}

#endif
