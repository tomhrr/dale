#ifndef DALE_ERRORTYPE
#define DALE_ERRORTYPE

namespace dale
{
/*! ErrorType

    Contains the error type and instance enums, and functions for
    dealing with them.  Error types are split into five categories:
    error, warning, diagnostic, debug, and internal.  Error instances
    are split into three categories: lexer, parser, and generator.
*/
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
enum
{
    Null = 1,
    UnterminatedStringLiteral,
    InvalidInteger,
    InvalidFloatingPointNumber,

    MissingLeftParen,
    MissingRightParen,
    ExpectedLeftParen,

    CannotRemoveInternalFunction,
    CannotRemoveInternalVariable,
    CannotFindSymbolForMacro,
    CannotCreateExecutionEngine,
    CannotLoadLibrary,

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
    UnexpectedElementWithoutActual,
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
    OnlyVarAndStructPermitted,
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
    RefsNotPermittedHere,
    RetvalsNotPermittedHere,

    DNodeHasNoString,
    DNodeIsNeitherTokenNorList,

    NodeIsNeitherTokenNorList,
    NamespaceNotInContext,
    CannotDeactivateInactiveNamespace,
    CannotDeactivateNonLastNamespace,
    InvalidModuleName,
    OnlyOneModuleFormPermitted,
    MacroExpansionError,

    ExternalError,

    StructContainsPadding
};
}
/*! Get the error string for the given error instance.
 *  @param error_instance The error instance.
 *
 *  The returned string may contain printf placeholders.  The caller
 *  is expected to know the number and types of these placeholders.
 */
const char* errorInstanceToString(int error_instance);
/*! Get the error type for the given error instance.
 *  @param error_instance The error instance.
 */
int errorInstanceToType(int error_instance);
/*! Get the string for the given error type.
 *  @param error_type The error type.
 *
 *  This string is generally the enum name with a lowercase first
 *  letter.
 */
const char *errorTypeToString(int error_type);
}

#endif
