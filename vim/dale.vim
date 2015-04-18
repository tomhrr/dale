" This is based on (and mostly the same as) the lisp.vim syntax file
" that comes with Vim, which is maintained by Dr. Charles E. Campbell,
" Jr. <NdrOchipS@PcampbellAfamily.Mbiz>.  The only change made here,
" apart from updating the built-in function/declaration lists, is that
" multi-line comments may occur within lists.

if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

if exists("g:lispsyntax_ignorecase") || exists("g:lispsyntax_cdale")
  set ignorecase
endif

"set lisp
" Not so sure if this is a good idea.
set iskeyword+=@-@,42,43,45,47,60-62

" Clusters:
syn cluster     lispAtomCluster         contains=lispAtomBarSymbol,lispAtomList,lispAtomNmbr0,lispComment,lispDecl,lispFunc,lispLeadWhite
syn cluster     lispBaseListCluster     contains=lispAtom,lispAtomBarSymbol,lispAtomMark,lispBQList,lispBarSymbol,lispComment,lispCommentRegion,lispConcat,lispDecl,lispFunc,lispKey,lispList,lispNumber,lispEscapeSpecial,lispSymbol,lispVar,lispLeadWhite,lispType
if exists("g:lisp_instring")
    syn cluster    lispListCluster      contains=@lispBaseListCluster,lispString,lispInString,lispInStringString
else
    syn cluster    lispListCluster      contains=@lispBaseListCluster,lispString
endif

syn case ignore

" Lists:
syn match       lispSymbol          contained   ![^()'`,"; \t]\+!
syn match       lispBarSymbol       contained   !|..\{-}|!
if exists("g:lisp_rainbow") && g:lisp_rainbow != 0
 syn region lispParen0           matchgroup=hlLevel0 start="`\=(" end=")" skip="|.\{-}|" contains=@lispListCluster,lispParen1
 syn region lispParen1 contained matchgroup=hlLevel1 start="`\=(" end=")" skip="|.\{-}|" contains=@lispListCluster,lispParen2
 syn region lispParen2 contained matchgroup=hlLevel2 start="`\=(" end=")" skip="|.\{-}|" contains=@lispListCluster,lispParen3
 syn region lispParen3 contained matchgroup=hlLevel3 start="`\=(" end=")" skip="|.\{-}|" contains=@lispListCluster,lispParen4
 syn region lispParen4 contained matchgroup=hlLevel4 start="`\=(" end=")" skip="|.\{-}|" contains=@lispListCluster,lispParen5
 syn region lispParen5 contained matchgroup=hlLevel5 start="`\=(" end=")" skip="|.\{-}|" contains=@lispListCluster,lispParen6
 syn region lispParen6 contained matchgroup=hlLevel6 start="`\=(" end=")" skip="|.\{-}|" contains=@lispListCluster,lispParen7
 syn region lispParen7 contained matchgroup=hlLevel7 start="`\=(" end=")" skip="|.\{-}|" contains=@lispListCluster,lispParen8
 syn region lispParen8 contained matchgroup=hlLevel8 start="`\=(" end=")" skip="|.\{-}|" contains=@lispListCluster,lispParen9
 syn region lispParen9 contained matchgroup=hlLevel9 start="`\=(" end=")" skip="|.\{-}|" contains=@lispListCluster,lispParen0
else
 syn region lispList             matchgroup=Delimiter start="("   skip="|.\{-}|"            matchgroup=Delimiter end=")"    contains=@lispListCluster
 syn region lispBQList           matchgroup=PreProc   start="`("  skip="|.\{-}|"            matchgroup=PreProc   end=")"    contains=@lispListCluster
endif

" Atoms:
syn match lispAtomMark          "'"
syn match lispAtom              "'("me=e-1          contains=lispAtomMark    nextgroup=lispAtomList
syn match lispAtom              "'[^ \t()]\+"       contains=lispAtomMark
syn match lispAtomBarSymbol     !'|..\{-}|!         contains=lispAtomMark
syn region lispAtom             start=+'"+          skip=+\\"+ end=+"+
syn region lispAtomList         contained           matchgroup=Special start="("    skip="|.\{-}|" matchgroup=Special end=")"    contains=@lispAtomCluster,lispString,lispEscapeSpecial
syn match lispAtomNmbr          contained           "\<\d\+"
syn match lispLeadWhite         contained           "^\s\+"

" Strings:
syn region      lispString      start=+"+ skip=+\\\\\|\\"+ end=+"+    contains=@Spell
if exists("g:lisp_instring")
    syn region            lispInString              keepend matchgroup=Delimiter start=+"(+rs=s+1 skip=+|.\{-}|+ matchgroup=Delimiter end=+)"+ contains=@lispBaseListCluster,lispInStringString
    syn region            lispInStringString        start=+\\"+ skip=+\\\\+ end=+\\"+ contained
endif

" Numbers: supporting integers and floating point numbers
syn match lispNumber            "-\=\d\+"
syn match lispNumber            !0x\d\+!

syn match lispEscapeSpecial     !#\\\%("\|SPACE\|NEWLINE\|TAB\|CARRIAGE\|EOF\)!
syn match lispEscapeSpecial     "\<+[a-zA-Z_][a-zA-Z_0-9-]*+\>"

syn match lispConcat            "\s\.\s"
syn match lispParenError        ")"

" Comments:
syn cluster lispCommentGroup    contains=lispTodo,@Spell
syn match   lispComment         ";.*$"                  contains=@lispCommentGroup
syn region  lispCommentRegion   start="#|" end="|#"     contains=lispCommentRegion,@lispCommentGroup
syn keyword lispTodo            contained combak combak: todo todo:

syn keyword lispFunc            goto if label return setf @ : # $
syn keyword lispFunc            p+ p- p= p< p>
syn keyword lispFunc            va-arg va-start va-end
syn keyword lispFunc            null q def nullptr do cast
syn keyword lispFunc            sizeof offsetof alignmentof
syn keyword lispFunc            funcall using-namespace new-scope array-of
syn keyword lispFunc            setv @$ :@ @: @:@
syn keyword lispFunc            namespace include module import once

syn keyword lispDecl            var fn macro struct enum
syn keyword lispDecl            auto intern extern opaque

syn keyword lispType            void bool char ... float double long-double
syn keyword lispType            intptr size ptrdiff
syn keyword lispType            int  int8  int16  int32  int64  int128
syn keyword lispType            uint uint8 uint16 uint32 uint64 uint128
syn keyword lispType            const ref p

" Synchronization:
syn sync lines=100

" Define Highlighting:
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508
  command -nargs=+ HiLink hi def link <args>

  HiLink lispCommentRegion      lispComment
  HiLink lispAtomNmbr           lispNumber
  HiLink lispAtomMark           lispMark
  HiLink lispInStringString     lispString

  HiLink lispAtom               Identifier
  HiLink lispAtomBarSymbol      Special
  HiLink lispBarSymbol          Special
  HiLink lispComment            Comment
  HiLink lispConcat             Statement
  HiLink lispDecl               Statement
  HiLink lispFunc               Statement
  HiLink lispType               Type
  HiLink lispMark               Delimiter
  HiLink lispNumber             Number
  HiLink lispParenError         Error
  HiLink lispEscapeSpecial      Type
  HiLink lispString             String
  HiLink lispTodo               Todo
  HiLink lispVar                Statement

  if exists("g:lisp_rainbow") && g:lisp_rainbow != 0
   if &bg == "dark"
    hi def hlLevel0 ctermfg=red         guifg=red1
    hi def hlLevel1 ctermfg=yellow      guifg=orange1
    hi def hlLevel2 ctermfg=green       guifg=yellow1
    hi def hlLevel3 ctermfg=cyan        guifg=greenyellow
    hi def hlLevel4 ctermfg=magenta     guifg=green1
    hi def hlLevel5 ctermfg=red         guifg=springgreen1
    hi def hlLevel6 ctermfg=yellow      guifg=cyan1
    hi def hlLevel7 ctermfg=green       guifg=slateblue1
    hi def hlLevel8 ctermfg=cyan        guifg=magenta1
    hi def hlLevel9 ctermfg=magenta     guifg=purple1
   else
    hi def hlLevel0 ctermfg=red         guifg=red3
    hi def hlLevel1 ctermfg=darkyellow  guifg=orangered3
    hi def hlLevel2 ctermfg=darkgreen   guifg=orange2
    hi def hlLevel3 ctermfg=blue        guifg=yellow3
    hi def hlLevel4 ctermfg=darkmagenta guifg=olivedrab4
    hi def hlLevel5 ctermfg=red         guifg=green4
    hi def hlLevel6 ctermfg=darkyellow  guifg=paleturquoise3
    hi def hlLevel7 ctermfg=darkgreen   guifg=deepskyblue4
    hi def hlLevel8 ctermfg=blue        guifg=darkslateblue
    hi def hlLevel9 ctermfg=darkmagenta guifg=darkviolet
   endif
  endif

  delcommand HiLink
endif

let b:current_syntax = "dale"
