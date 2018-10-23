#ifndef DALE_ERRORTYPE
#define DALE_ERRORTYPE

namespace dale {
/*! ErrorType

    Contains the error type and instance enums, and functions for
    dealing with them.  Error types are split into five categories:
    error, warning, diagnostic, debug, and internal.  Error instances
    are split into three categories: lexer, parser, and generator.
*/
namespace ErrorType {
enum { Error = 1, Warning, Diagnostic, Debug, Internal };
}

namespace ErrorInst {
enum {
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

    ExpectedList,
    OnlyListsAtTopLevel,
    NoEmptyLists,
    FirstListElementMustBeAtom,
    FirstListElementMustBeAtomOrMacroOrFP,
    FirstListElementMustBeSymbol,
    NoEmptyDo,
    IncorrectNumberOfArgs,
    IncorrectMinimumNumberOfArgs,
    IncorrectMaximumNumberOfArgs,
    IncorrectArgType,
    IncorrectSingleParameterType,
    IncorrectParameterTypeNumberOfArgs,
    OverloadedFunctionOrMacroNotInScope,
    OverloadedFunctionOrMacroNotInScopeWithClosest,
    OverloadedFunctionOrMacroNotInScopeWithClosestFirstMatches,
    NamespaceNotInScope,
    TypeNotInScope,
    MacroNotInScope,
    FunctionNotInScope,
    VariableNotInScope,
    NotInScope,
    UnexpectedElement,
    VoidMustBeTheOnlyParameter,
    VarArgsMustBeLastParameter,
    VarArgsNotPermittedForMacros,
    TypeNotAllowedInStruct,
    CannotInitialiseExternVar,
    InvalidType,
    CannotParseLiteral,
    RedeclarationOfStruct,
    RedefinitionOfVariable,
    RedeclarationOfLabel,
    RedeclarationOfFunctionOrMacro,
    RedeclarationOfDifferentKind,
    NonPointerFunctionParameter,
    ExternCInNamespace,
    IncorrectType,
    IncorrectReturnType,
    CannotDereferenceNonPointer,
    CannotDereferenceVoidPointer,
    CannotDereferencePointerInContext,
    OverloadedFunctionAddressWithoutTypes,
    NoSuchFileOrDirectory,
    FileError,
    InvalidCast,
    FieldDoesNotExistInStruct,
    OnlyVarAndStructPermitted,
    UnableToParseForm,
    InvalidChar,
    TypeNotSupportedForMacro,
    GotNullPointerFromMacroCall,
    LabelNotInScope,
    UnableToParseInteger,
    UnableToParseIntegerNoString,
    UnableToConvertTypeToLLVMType,
    TypeIsNotFirstClass,
    ArraysCannotBeFunctionParameters,
    IncorrectNumberOfArrayElements,
    CanOnlyIndexIntoPointersAndArrays,
    ExternCXorOtherLinkageForName,
    ReturnTypesCannotBeArrays,
    CannotTakeAddressOfNonLvalue,
    MustHaveInitialiserForImpliedType,
    MustHaveInitialiserForConstType,
    MustHaveInitialiserForType,
    UnableToResolvePointerAddress,
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
    CopyDisabled,
    RefsNotPermittedHere,
    RetvalsNotPermittedHere,
    CTOFromNonCTO,
    CTOAddrFromNonCTO,
    CTOAnonymousFromNonCTO,

    DNodeHasNoString,
    DNodeIsNeitherTokenNorList,

    NodeIsNeitherTokenNorList,
    NamespaceNotInContext,
    CannotDeactivateInactiveNamespace,
    CannotDeactivateNonLastNamespace,
    InvalidModuleName,
    OnlyOneModuleFormPermitted,
    MacroExpansionError,
    ArityForExternCOnly,
    NoRefsInExternC,
    UnsupportedEvalExpressionType,

    ExternalError,

    StructContainsPadding,
    MustHaveConstantInitialiser
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
const char* errorTypeToString(int error_type);
}

#endif
